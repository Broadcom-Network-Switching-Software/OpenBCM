/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * the cint creates 2 ports of types: <type1>,<type2>, each port with/out protection
 * of type facility/path and then cross-connects the 2 ports.
 *  
 * optional ports combinations:
 * vlan - vlan
 * vlan - mpls
 * vlan - mim
 * mpls - mim
 *  
 * type: 1 = vlan, 2 = mpls, 3 = mim, 4 = mim (port type peer)
 * with_protection: 1 = with, 0 = without 
 * facility_protection: 1 = facitilty type protection, 0 = path
 *  
 * run: 
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_mpls_port.c
 * cint utility/cint_sand_utils_multicast.c 
 * cint cint_port_tpid.c
 * cint cint_advanced_vlan_translation_mode.c
 * cint cint_mpls_lsr.c
 * cint cint_vswitch_metro_mp.c
 * cint cint_vswitch_cross_connect_p2p.c
 * cint
 * run(0, <port1>, <type1>, <with_protection_1>, <second_port1>, <port2>, <type2>, <with_protection_2>, <second_port2>, <facility_protection>); 
 *  - with_protection_1 == 0 ==> no protection
 *  - with_protection_1 == 1 ==> 1:1 protection
 *  - with_protection_1 == 2 ==> 1+1 protection
 *  
 *  with_protection_2 can be only 0 or 1.
 *  
 *  if with_protection:
 *  second_port1, second_port2 will be used as well.
 *  
 * run packets: 
 * from vlan port (port1): 
 *      ethernet header with any DA, SA
 *      vlan tag with vlan tag id 100 (without protection) or 101 (with protection)
 *  
 * for case vlan - vlan, second vlan port (port2): 
 *      ethernet header with any DA, SA
 *      vlan tag with vlan tag id 101 (both without protection) ,102 (one with protection) or 103 (both with protection)
 *  
 * from mpls port (port1): 
 *      ethernet header with DA 11 and any SA
 *      vlan tag with vlan tag id 10 (without protection) or 11 (with protection)
 *      mpls header with label 20 (without protection) or 21 (with protection)
 *  
 * from mim port (port1): 
 *      mim header with B-DA abcdef123412, B-SA 010203040506
 *      B-tag with B-VID 5 (without protection) or 6 (with protection), ethertype 0x81a8
 *      I-tag with I-SID 5555 (without protection) or 5556 (with protection), ethertype 0x88b5
 *  
 * the packet will arrive at the port that is cross-connected to the port it was sent from 
 * when arrives at vlan port: 
 *      with vlan tag id 1000 (without protection) or 1001 (with protection)
 * for case vlan - vlan, when arrives at second vlan port: 
 *      with vlan tag id 1001 (both without protection) ,1002 (one with protection) or 1003 (both with protection)
 *  
 * when arrives at mpls port: 
 *      with ethernet header with DA 22, SA 11 
 *      mpls header with label1: 40, exp 0, ttl 40
 *                       label2: 20, exp 0, ttl 20
 *                       label3: 40, exp 0(0), ttl 40(20) (without protection)
 *      or mpls header with label1: 41, exp 0, ttl 40
 *                          label2: 21, exp 0, ttl 20
 *                          label3: 41, exp 0, ttl 40 (with protection)
 *  
 * when arrives at mim port: 
 *      with mim header with B-DA abcdef123412, B-SA abcdef123400 
 *      B-tag with B-VID 5 (without protection) or 6 (with protection), ethertype 0x81a8
 *      I-tag with I-SID 5555 (without protection) or 5556 (with protection), ethertype 0x88b5
 *  
 * if protection exists: 
 *      if the port that is cross-connected will fail, packets will arrive at the protection port
 *  
 * run: 
 * traverse_run(0); - to traverse and print all the cross-connected ports 
 * delete_all(0); - to delete all the cross-connections 
 */

int verbose = 3;
int vswitch_p2p_encap_id = 0; /* global variable that saves encap id of mpls_port created in mpls_port_create*/
int vswitch_p2p_create_mpls_port_without_fec_and_replace = 0; /* global variable that indicates whether to create a mpls port without fec (and then replace it) */
int configuration_hub = 0; /* set the vlan port to be HUB */
int decoupled_mode = 0; /* global variable that determines whether we are using decoupled mode in protection*/
int index_value = 2;  /* global variable that sets the In-VC label termination index when SOC properties mpls_termination_label_index_enable is enabled */

struct cross_connect_info_s {

    /* physical ports */
    bcm_port_t port_1; 
    bcm_port_t port_2; 
    bcm_port_t port_3; 
    bcm_port_t port_4; 

    bcm_port_t port_5; /* for traverse */

    /* logical ports */
    bcm_gport_t vlan_port_1; /* vlan ports */
    bcm_gport_t vlan_port_2; 
    bcm_mim_port_t mim_port_1; /* mim port */
    bcm_mpls_port_t mpls_port_1; /* mpls port */

    /* logical ports for protection */
    bcm_gport_t vlan_port_3; 
    bcm_gport_t vlan_port_4; 
    bcm_mim_port_t mim_port_2;
    bcm_mpls_port_t mpls_port_2;

    bcm_vswitch_cross_connect_t gports;
    /* 1+1 protection port*/
    bcm_gport_t prim_gport_1;

    /* data for vlan port */
    bcm_vlan_t outer_vlan_base;
    bcm_vlan_t eg_vlan_base;

    /* data for mim port */
    bcm_mim_vpn_config_t bvid_1; /* B-VIDs */
    bcm_mim_vpn_config_t bvid_2;
    bcm_mim_vpn_t bvid_base;
    int isid_base; /* I-SID */
    bcm_mac_t src_bmac; /* B-SA */
    bcm_mac_t dest_bmac; /* B-DA */
    
    /* data for mpls port */
    int in_vc_label_base; /* base for incomming VC label */
    int eg_vc_label_base; /* base for egress VC label */
    int vlan_base;
    int mpls_eg_vlan_base;
    uint8 my_mac[6];
    uint8 nh_mac[6]; /* next hop mac address */

    /* MC for 1+1 protection*/
    int protection_mc1;
    int protection_mc2;

    /* failover IDs for protection */
    bcm_failover_t failover_id_1;
    bcm_failover_t failover_id_2;
    bcm_failover_t failover_id_3;

    /** FEC for protection*/
    bcm_gport_t ac_fec_gport;
    bcm_gport_t pwe_fec_gport;
};

cross_connect_info_s cross_connect_info;

void
cross_connect_info_init(int port1, int second_port1, int port2, int second_port2) {

    bcm_vswitch_cross_connect_t_init(&cross_connect_info.gports);

    cross_connect_info.port_1 = port1; 
    cross_connect_info.port_2 = port2;
    cross_connect_info.port_3 = second_port1; 
    cross_connect_info.port_4 = second_port2;

    cross_connect_info.port_5 = 5;

    cross_connect_info.outer_vlan_base = 100;
    cross_connect_info.eg_vlan_base = 1000;

    cross_connect_info.bvid_base = 5;
    cross_connect_info.isid_base = 5555;

    cross_connect_info.src_bmac[0] = 0x01;
    cross_connect_info.src_bmac[1] = 0x02;
    cross_connect_info.src_bmac[2] = 0x03;
    cross_connect_info.src_bmac[3] = 0x04;
    cross_connect_info.src_bmac[4] = 0x05;
    cross_connect_info.src_bmac[5] = 0x06;

    cross_connect_info.dest_bmac[0] = 0xab;
    cross_connect_info.dest_bmac[1] = 0xcd;
    cross_connect_info.dest_bmac[2] = 0xef;
    cross_connect_info.dest_bmac[3] = 0x12;
    cross_connect_info.dest_bmac[4] = 0x34;
    cross_connect_info.dest_bmac[5] = 0x12;

    cross_connect_info.in_vc_label_base = 20;
    cross_connect_info.eg_vc_label_base = 40; 
    cross_connect_info.vlan_base = 10;
    cross_connect_info.mpls_eg_vlan_base = 20;

    cross_connect_info.my_mac[0] = 0x00;
    cross_connect_info.my_mac[1] = 0x00;
    cross_connect_info.my_mac[2] = 0x00;
    cross_connect_info.my_mac[3] = 0x00;
    cross_connect_info.my_mac[4] = 0x00;
    cross_connect_info.my_mac[5] = 0x11;

    cross_connect_info.nh_mac[0] = 0x00;
    cross_connect_info.nh_mac[1] = 0x00;
    cross_connect_info.nh_mac[2] = 0x00;
    cross_connect_info.nh_mac[3] = 0x00;
    cross_connect_info.nh_mac[4] = 0x00;
    cross_connect_info.nh_mac[5] = 0x22;

    cross_connect_info.protection_mc1 = 6001;
    cross_connect_info.protection_mc2 = 6002;

    cross_connect_info.failover_id_1 = 0;
    cross_connect_info.failover_id_2 = 0;
    cross_connect_info.failover_id_3 = 0;

    cross_connect_info.ac_fec_gport = BCM_GPORT_INVALID;
    cross_connect_info.pwe_fec_gport = BCM_GPORT_INVALID;
}

int space_optimize_enable = 0;

/* create vlan_port (Logical interface identified by port-vlan-vlan) */
int
vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t match_vlan, /* outer vlan */
    bcm_vlan_t egress_vlan,
    bcm_gport_t *gport,
    int failover_id,
    bcm_gport_t protect_gport,
    int protect_mc,
    int dbl_tagged
    ){

    int rv;
    bcm_vlan_port_t vp;
    bcm_pbmp_t pbmp, ubmp;
    bcm_gport_t fec_gport_id = BCM_GPORT_INVALID;

    rv = bcm_vlan_create(unit, match_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", match_vlan);
    }

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = dbl_tagged ? BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED : BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = match_vlan; 
    vp.match_inner_vlan = dbl_tagged ? 40 : 0;
    vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : egress_vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.egress_inner_vlan = is_device_or_above(unit, JERICHO2) ? 0 : (dbl_tagged ? 40 : 0);
    vp.vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    vp.flags = configuration_hub ? BCM_VLAN_PORT_NETWORK : 0;
  
    /* distinguish between ingress/egress failover according if MC is set*/
    if (protect_mc == 0) {
        vp.failover_id = failover_id;
    }
    else{
        vp.ingress_failover_id = failover_id;
        vp.ingress_failover_port_id = protect_gport;
    }

    if (is_device_or_above(unit, JERICHO2)) {

        /** In JR2, In creating AC it should be 0 since it is used in creating FEC.*/
        vp.failover_mc_group = 0;

        /** JR2: create a FEC for storing bi-cast mc group for 1+1 protection. The FEC will be set in learn-info*/
        if (vp.ingress_failover_id || vp.failover_mc_group) {
            if (protect_gport == 0) {
                /** Standby: The FEC was not created yet, create it here.*/
                create_l3_egress_s l3_egress;
                sal_memset(&l3_egress, 0, sizeof(l3_egress));

                BCM_GPORT_MCAST_SET(l3_egress.out_gport, protect_mc);
                rv = l3__egress_only_fec__create(unit, &l3_egress);
                if (rv != BCM_E_NONE) {
                    printf("Error, l3__egress_only_fec__create for FEC\n");
                    return rv;
                }
                BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport_id, l3_egress.fec_id);

                vp.failover_port_id = fec_gport_id;

                /** store it for primary path*/
                cross_connect_info.ac_fec_gport = fec_gport_id;
            } else {
                /** Primary: The FEC was created already in standby path, use it.*/
                vp.failover_port_id = cross_connect_info.ac_fec_gport;
            }
        }
    } else {
        vp.failover_mc_group = protect_mc; 

        /** JR1: FEC is created in standby AC creation, primary get the FEC from standby AC.*/
        vp.failover_port_id = protect_gport;
    }

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    if(verbose >= 2) {
        printf("created vlan-port:0x0%8x  \n", vp.vlan_port_id);
        printf("  encap-id: 0x%8x  \n", vp.encap_id);
        printf("  in-port: %d\n", vp.port);
        printf("  in-vlan: %d\n", vp.match_vlan);
    }
    /* egress_vlan and egress_inner_vlan are used at eve */
    vp.egress_vlan = egress_vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.egress_inner_vlan = dbl_tagged ? 40 : 0;

    /* return port id to user */
    *gport = vp.vlan_port_id;
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }

    /** JR2: Add port to vlan since the vlan membership check was enabled by default*/
    if (!advanced_vlan_translation_mode || is_device_or_above(unit, JERICHO2)) {
        BCM_PBMP_CLEAR(pbmp); /* tagged */
        BCM_PBMP_CLEAR(ubmp); /* untagged */
        BCM_PBMP_PORT_ADD(pbmp, vp.port);
        
        /* set vlan port membership */
        rv = bcm_vlan_port_add(unit, match_vlan, pbmp, ubmp);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_add with vlan %d\n", match_vlan);
            print rv;
            return rv;
        }
    }

    return BCM_E_NONE;
}

/**
 * Cross connect port pin, vlan vid to port pout vlan vid+1. 
 * The gports (lifs) of the ports are returned in gport{1,2}. 
 */
int cross_connect(int unit, int p1, int *gport1, int p2, int *gport2, int vid)
{
    int rv;
    bcm_vlan_port_t vp;
    int i;
    int ports[2];
    int *gport_out_arr[2];
    bcm_vswitch_cross_connect_t xconn_info;

    if (verbose >= 2) {
        printf("Creating vlan %d...\n", vid);
    }

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", vid);
    }
    rv = bcm_vlan_create(unit, vid+1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", vid+1);
    }

    ports[0] = p1;
    ports[1] = p2;
    gport_out_arr[0] = gport1;
    gport_out_arr[1] = gport2;

    for (i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = ports[i];
        /* +i - This is instead of setting different port domains for the ports. */
        /* Otherwise, both port domains and vlans will be equal and thus we will have equal lifs. */
        vp.match_vlan = vid+i; 
        vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : (vid+i);
        vp.vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
        vp.flags = 0;

        if (verbose >= 2) {
            printf("Adding port %d to vlan %d...\n", ports[i], vid+i);
        }
        
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            print rv;
            return rv;
        }

        printf("The gport for port %d, vlan %d is 0x%x.\n", vp.port, vp.match_vlan, vp.vlan_port_id);
        *(gport_out_arr[i]) = vp.vlan_port_id;

    }

    bcm_vswitch_cross_connect_t_init(&xconn_info);
    xconn_info.port1 = *gport1;
    xconn_info.port2 = *gport2;

    if (verbose >= 2) {
        printf("Cross connecting the ports...\n");
    }
    rv = bcm_vswitch_cross_connect_add(unit, &xconn_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add.\n");
        print rv;
        return rv;
    }
    
    return BCM_E_NONE;
}

int vswitch_cross_connect_double_tag_ac_lif_configuration(int unit,bcm_port_t port,bcm_vlan_t vlan){
    int mpls_termination_label_index_enable;
    int mpls_termination_label_index_database_mode;
    int rv=0;

	/* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    /* indicate new VTT mode */
	mpls_termination_label_index_database_mode = soc_property_get(unit , spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 0);
 
    if ((mpls_termination_label_index_enable == 1) &&
		((mpls_termination_label_index_database_mode == 22) || (mpls_termination_label_index_database_mode == 23))) {
		bcm_vlan_port_t vlan_port_1;
		bcm_gport_t vlan_port_id = 0;

		/* add port, according to port_vlan_vlan */
		bcm_vlan_port_t_init(&vlan_port_1);

		/* set port attributes, key <port-vlan-vlan>*/
		vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
		vlan_port_1.vsi = cross_connect_info.mpls_eg_vlan_base;
		vlan_port_1.port = port;
		vlan_port_1.match_vlan = vlan;
		vlan_port_1.match_inner_vlan = 40;
		vlan_port_1.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : cross_connect_info.mpls_eg_vlan_base;
		vlan_port_1.egress_inner_vlan = 40;
		vlan_port_1.flags = 0;

		rv = bcm_vlan_port_create(unit, &vlan_port_1);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_vlan_port_create\n");
			return rv;
		}

		printf("Unit %d: vlan_port_1.vlan_port_id 0x%08x vlan_port_encap_id1: 0x%08x\n\r", unit, vlan_port_1.vlan_port_id, vlan_port_1.encap_id);
        /* egress_vlan is used at eve */
		vlan_port_1.egress_vlan = cross_connect_info.mpls_eg_vlan_base;
		/* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
		   action mapping. This is here to compensate. */
		if (advanced_vlan_translation_mode) {
			rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port_1);
			if (rv != BCM_E_NONE) {
				printf("Error: vlan_translation_vlan_port_create_to_translation\n");
			}
		}
	}
    return rv;
}

/* initialize the tunnels for mpls routing */
int
mpls_tunnels_config(
    int unit,
    bcm_port_t port,
    int *egress_intf, 
    int in_tunnel_label,
    int out_tunnel_label){

    int ingress_intf;
    int encap_id;
    bcm_mpls_egress_label_t label_array[2];
    bcm_pbmp_t pbmp;
    int rv;
    bcm_vlan_t vlan, eg_vlan;
 
    /* open vlan */
    vlan = cross_connect_info.vlan_base++;
    printf("open vlan %d\n", vlan);

    /* configure the AC lif for double tag packets - this function relevant for termination mode 22-23 only */
    rv = vswitch_cross_connect_double_tag_ac_lif_configuration(unit,port,vlan);
	if (rv != BCM_E_NONE) {
		printf("Error, in vswitch_cross_connect_double_tag_ac_lif_configuration \n");
		print rv;
	}

    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", vlan);
        print rv;
    }

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_port_add(unit, vlan, pbmp, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        print rv;
        return rv;
    }
 
    /* l2 termination for mpls routing: packet received on those VID+MAC will be L2 terminated  */
    create_l3_intf_s intf;
    intf.vsi = vlan;
    intf.my_global_mac = cross_connect_info.my_mac;
    intf.my_lsb_mac = cross_connect_info.my_mac;
    intf.skip_mymac = 1;
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
    
    /* create ingress object, packet will be routed to */
    eg_vlan = cross_connect_info.mpls_eg_vlan_base++;
    
    intf.vsi = eg_vlan;
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }

    /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    label_array[0].exp = 0; /* Set it statically 0*/ 
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[0].label = in_tunnel_label;
    label_array[0].ttl = 20;

    label_array[1].exp = 0; /* Set it statically 0 */ 
    label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[1].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    label_array[1].exp = 0; /* Set it statically 0*/ 
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[1].label = out_tunnel_label;
    label_array[1].ttl = 40;

    if (!is_device_or_above(unit, JERICHO2)) {
        /** In JR1, l3_intf_id is used for RIF or next tunnel-id.*/
        label_array[1].l3_intf_id = ingress_intf;
    } else {
        /** In JR2, l3_intf_id is used for next-outlif which will be set in creating ARP.*/
        label_array[1].l3_intf_id = 0;
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        label_array[1].l3_intf_id = ingress_intf;
        rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    } else {
        /** In JR2, l3_intf_id is used for next-outlif. It must be LIF type.*/
        label_array[1].l3_intf_id = 0;

        /** To avoid failing leagcy tests, we call sand API in JR2 only.*/
        rv = sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    }

    /* create egress object points to this tunnel/interface */
    ingress_intf = label_array[0].tunnel_id;
    create_l3_egress_s l3eg;
    
    l3eg.out_tunnel_or_rif = ingress_intf;
    sal_memcpy(l3eg.next_hop_mac_addr, cross_connect_info.nh_mac, 6);
    l3eg.vlan   = eg_vlan;
    l3eg.out_gport   = port;
    l3eg.arp_encap_id = encap_id;
    l3eg.fec_id = *egress_intf; 
    l3eg.allocation_flags = 0;
   
    if (space_optimize_enable == 1) {
        l3eg.l3_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }
 
    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", vlan);
        print rv;
    }
    
    encap_id = l3eg.arp_encap_id;
    *egress_intf = l3eg.fec_id;
  
    return rv;
}

int 
mpls_port_create(
    int unit, 
    bcm_port_t port, 
    bcm_mpls_port_t *mpls_port,
    int failover_id,
    bcm_gport_t protect_gport,
    int protect_mc) {

    int rv;
    int egress_intf;
    int in_vc_label = cross_connect_info.in_vc_label_base++;
    int eg_vc_label = cross_connect_info.eg_vc_label_base++;
    int mpls_termination_label_index_enable;
    int mpls_termination_label_index_database_mode;
    
    rv = mpls_tunnels_config(unit, port, &egress_intf, in_vc_label, eg_vc_label);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnels_config\n");
        return rv;
    }

    /* add port, according to VC label */
    bcm_mpls_port_t_init(mpls_port);
  
    /* set port attribures */
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port->match_label = in_vc_label;
    /* if vswitch_p2p_create_mpls_port_without_fec_and_replace==0, create mpls port with no fec
       otherwise, create with fec */
    if (!vswitch_p2p_create_mpls_port_without_fec_and_replace) {
        mpls_port->egress_tunnel_if = egress_intf;
    } else {
        mpls_port->flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    }
    mpls_port->flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port->port = port;
    mpls_port->egress_label.label = eg_vc_label;
    mpls_port->failover_mc_group = protect_mc;
    if (is_device_or_above(unit, JERICHO2)) {
        mpls_port->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

        /** JR2 use ingress_* for 1+1 receiving selection in egress node*/
        mpls_port->ingress_failover_id = failover_id;
        mpls_port->ingress_failover_port_id = protect_gport;
    } else {
        mpls_port->egress_label.flags |= 0;
        mpls_port->failover_id = failover_id;
        mpls_port->failover_port_id = protect_gport;
    }
    

    if (configuration_hub) {
        mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
    }

    rv = mpls_port__update_network_group_id(unit, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port_create\n");
        return rv;
    }

    /* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    /* indicate new VTT mode */
    mpls_termination_label_index_database_mode = soc_property_get(unit , spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 0);

    if (mpls_termination_label_index_enable) {
        /* PWE is Application label */
        if ((mpls_termination_label_index_database_mode >= 20) && (mpls_termination_label_index_database_mode <= 21)) {
            index_value = 3;
        }
        BCM_MPLS_INDEXED_LABEL_SET(&mpls_port->match_label,in_vc_label,index_value);
    }
  
    /* to create a p2p mpls port that is not connected to p2p vpn - use vpn=0 */
    if (is_device_or_above(unit, JERICHO2))
    {
        mpls_port->flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;
        /** to avoid updating in loading cints, we call sand_mpls_port_add in JR2 only.*/
        rv = sand_mpls_port_add(unit, 0, mpls_port);
    }
    else
    {
        rv = bcm_mpls_port_add(unit, 0, mpls_port);
    }

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        print rv;
        return rv;
    }     

    if (vswitch_p2p_create_mpls_port_without_fec_and_replace) {
        /* Replace the above created mpls port (without fec) with a new mpls port(with fec)  */
        mpls_port->flags = BCM_MPLS_PORT_REPLACE|BCM_MPLS_PORT_WITH_ID|BCM_MPLS_PORT_EGRESS_TUNNEL;
        mpls_port->flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
        mpls_port->encap_id = 0;
        mpls_port->egress_tunnel_if = egress_intf;
        rv = bcm_mpls_port_add(unit, 0, mpls_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add\n");
            print rv;
            return rv;
        }
    }

    vswitch_p2p_encap_id = mpls_port->encap_id;

    return BCM_E_NONE;
}

int 
mim_l2_station_add(
    int unit,
    int global, /* global configuration of per port */
    int port,
    bcm_mac_t mac
    ) {

    int rv, station_id;
    bcm_l2_station_t station; 

    bcm_l2_station_t_init(&station); 

    station.flags = BCM_L2_STATION_MIM;
    sal_memcpy(station.dst_mac, mac, 6);

    if (global) { /* global configuration */
        station.src_port_mask = 0; /* port is not valid */
        station.dst_mac_mask[0] = 0xff; /* dst_mac MSB should be used */
        station.dst_mac_mask[1] = 0xff;
        station.dst_mac_mask[2] = 0xff;
        station.dst_mac_mask[3] = 0xff;
        station.dst_mac_mask[4] = 0xff;
        station.dst_mac_mask[5] = 0x0;
    }
    else { /* per port configuration */
        station.src_port = port;
        station.src_port_mask = -1; /* port is valid */
        station.dst_mac_mask[0] = 0x0; /* dst_mac LSB should be used */
        station.dst_mac_mask[1] = 0x0;
        station.dst_mac_mask[2] = 0x0;
        station.dst_mac_mask[3] = 0x0;
        station.dst_mac_mask[4] = 0x0;
        station.dst_mac_mask[5] = 0xff;
    }

    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_station_add\n");
        print rv;
    }
    return BCM_E_NONE;
}

int 
mim_l2_station_add_for_termination(
    int unit,
    bcm_mac_t mac
    ) {

    int rv, station_id;
    bcm_l2_station_t station; 

    bcm_l2_station_t_init(&station); 

    station.flags = BCM_L2_STATION_MIM;
    sal_memcpy(station.dst_mac, mac, 6);

    station.src_port_mask = 0; 
    station.dst_mac_mask[0] = 0x0; /* dst_mac LSB should be used */
    station.dst_mac_mask[1] = 0x0;
    station.dst_mac_mask[2] = 0x0;
    station.dst_mac_mask[3] = 0x0;
    station.dst_mac_mask[4] = 0x0;
    station.dst_mac_mask[5] = 0xff;

    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_station_add\n");
        print rv;
    }
    return BCM_E_NONE;
}

int
mim_bvid_config(
    int unit,
    bcm_mim_vpn_config_t *vpn_config, 
    int vpn_id /* VPN ID */ 
    ) {

    int rv;
    bcm_multicast_t mc_group; /* mc_group = bc_group = uc_group */

    bcm_mim_vpn_config_t_init(vpn_config);

    vpn_config->flags |= BCM_MIM_VPN_WITH_ID; 
    vpn_config->flags |= BCM_MIM_VPN_BVLAN; /* set vpn as B-VID */
    vpn_config->vpn = vpn_id;

    /* open multicast group for the VPN */
    mc_group = vpn_id + 12*1024;  /* for B-VID mc_group = vpn + 12k */

    /* destroy before open, to ensure group does not exist */
    rv = bcm_multicast_destroy(unit, mc_group);
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        printf("Error, bcm_multicast_destroy, mc_group $mc_group\n");
        print rv;
        return rv;
    }

    /* create ingress MC group of type mim */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_TYPE_MIM, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create, mc_group $mc_group\n");
        print rv; 
        return rv;
    }
    
    vpn_config->broadcast_group = mc_group;
    vpn_config->unknown_unicast_group = mc_group;
    vpn_config->unknown_multicast_group = mc_group;

    return BCM_E_NONE;
}

int
mim_bvid_create(
    int unit,
    bcm_mim_vpn_config_t *vpn_config,
    int vpn_id, /* VPN ID */ 
    bcm_pbmp_t pbmp, /* port bit map */
    bcm_pbmp_t ubmp /* port bit map for untagged ports */
    ) {

    int rv;

    rv = mim_bvid_config(unit, vpn_config, vpn_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_config\n");
        return rv;
    }

    /* create the B-VID */
    rv = bcm_mim_vpn_create(unit, vpn_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_vpn_create\n");
        print rv;
        return rv;
    }
  
    /* set the B-VID port membership */
    rv = bcm_vlan_port_add(unit, vpn_config->vpn, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

/* configure bcm_mim_port_t with the given parameters */
void
mim_port_config(
    bcm_mim_port_t *mim_port, 
    int port, /* Physical port */
    uint16 vpn, /* B-VID */
    bcm_mac_t src_mac, /* In Source MAC address */ 
    uint16 bvid, /* Out B-VID */
    bcm_mac_t dest_mac, /* Out Dest MAC address */
    uint32 isid /* Egress I-SID - relevant for P2P ports only */ 
    ) {

    bcm_mim_port_t_init(mim_port);

    mim_port->flags = 0;
    mim_port->port = port;
    mim_port->match_tunnel_vlan = vpn;
    sal_memcpy(mim_port->match_tunnel_srcmac, src_mac, 6);
    mim_port->egress_tunnel_vlan = bvid; 
    sal_memcpy(mim_port->egress_tunnel_dstmac, dest_mac, 6);
    mim_port->egress_tunnel_service = isid;
}

/* function to check if the device is Jericho and above */
int device_is_jer(int unit,
                  uint8* is_jer) {
    bcm_info_t info;
    char *proc_name;

    proc_name = "device_is_jer";
    printf("%s(): Checking if device is Jericho...\n", proc_name);
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_info_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" info.device = 0x%x...\n", info.device);
    *is_jer = ((info.device == 0x8375) || (info.device == 0x8675));
    if (*is_jer) {
        printf("%s(): Done. Device is Jericho.\n", proc_name);
    } else {
        printf("%s(): Done. Device is not Jericho.\n", proc_name);
    }
    return rv;
}

int 
mim_port_create(
    int unit, 
    bcm_port_t port,
    bcm_mim_port_t *mim_port,
    bcm_mim_vpn_config_t *bvid, /* in and out B-VID */
    int isid, /* 2 mim ports cannot have the same I-SID */
    int failover_id,
    bcm_gport_t protect_gport,
    int is_type_peer
    ) {

    int rv;
    bcm_pbmp_t pbmp, ubmp;
    bcm_vlan_action_set_t action;
    uint8 is_jer;
    int mim_p2p_vsi;

    /* First check if this is a Jericho device which has the MiM erratum and assign the VSI accordingly */
    device_is_jer(unit, &is_jer);
    mim_p2p_vsi = (is_jer) ? 0x7FFF : 0xFFFF;

    /* set physical port as mim (backbone) port */
    rv = bcm_port_control_set(unit, port, bcmPortControlMacInMac, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        print rv;
        return rv;
    }

    /* make the mim physical port recognize the B-tag ethertype */
    port_tpid_init(port, 1, 0);
    port_tpid_info1.outer_tpids[0] = 0x81a8;
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_1\n");
        print rv;
        return rv;
    }


    /* set MiM port ingress vlan-editing to remove B-tag if exists 
    rv = bcm_vlan_port_default_action_set(unit, port, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_set with port_2\n");
        print rv;
        return rv;
    }*/

    /* prepare the port bit maps to set the B-VID port membership, with the the backbone port (used for filtering) */ 
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    bcm_mim_vpn_config_t_init(bvid);

    /* create B-VID */
    rv = mim_bvid_create(unit, bvid, cross_connect_info.bvid_base++, pbmp, ubmp); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with bvid\n");
        return rv;
    }

    /* create MiM (backbone) P2P port */
    mim_port_config(mim_port, port, bvid->vpn, cross_connect_info.src_bmac, bvid->vpn, cross_connect_info.dest_bmac, 
                    isid /* I-SID for P2P port */);
    
    /* add protection info */
    mim_port->failover_id = failover_id;
    mim_port->failover_gport_id = protect_gport; 

    if(is_type_peer) {
        mim_port->flags |= BCM_MIM_PORT_TYPE_PEER;
    }

    rv = bcm_mim_port_add(unit, mim_p2p_vsi, mim_port); /* use invalid VPN (=0xffff/0x7fff) to create P2P port depending on device type*/
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int 
mim_init(int unit) {

    int rv;
    bcm_pbmp_t pbmp, ubmp;

    /* init mim on the device */
    rv = bcm_mim_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_init\n");
        print rv;
        return rv;
    }

    /* set the global and per port L2 station for the B-DA (MyMac LSB + MSB) */
    rv = mim_l2_station_add(unit, 1 /* global */, 0, cross_connect_info.dest_bmac);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global)\n");
        print rv;
        return rv;
    }
    rv = mim_l2_station_add_for_termination(unit, cross_connect_info.dest_bmac);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add_for_termination\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int vswitch_cross_connect_double_tag_port_configuration(int unit, int port1, int protection1, int second_port1, int port2, int protection2, int second_port2,int *dbl_tag){

    int mpls_termination_label_index_enable;
    int mpls_termination_label_index_database_mode;
    int rv=0;
    
    /* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    mpls_termination_label_index_database_mode = soc_property_get(unit , spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 0);

    if ((mpls_termination_label_index_enable == 1) &&
        ((mpls_termination_label_index_database_mode == 22) || (mpls_termination_label_index_database_mode == 23))) {
        rv = bcm_vlan_control_port_set(unit, port1, bcmVlanPortDoubleLookupEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

        /* when a port is configured with  "bcmVlanPortDoubleLookupEnable" the VLAN domain must be unique in the device */
        rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set %d\n", rv);
            return rv;
        }

        rv = bcm_vlan_control_port_set(unit, port2, bcmVlanPortDoubleLookupEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set %d\n", rv);
            return rv;
        }

        if (protection1) {
            rv = bcm_vlan_control_port_set(unit, second_port1, bcmVlanPortDoubleLookupEnable, 1);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_port_class_set(unit, second_port1, bcmPortClassId, second_port1);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set %d\n", rv);
                return rv;
            }
        }

        if (protection2) {
            rv = bcm_vlan_control_port_set(unit, second_port2, bcmVlanPortDoubleLookupEnable, 1);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_port_class_set(unit, second_port2, bcmPortClassId, second_port2);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set %d\n", rv);
                return rv;
            }
        }

        *dbl_tag = 1;
    } else if (is_device_or_above(unit, JERICHO2)) {
        /*
         * In JR2, vlan_domain was set to port_id by default during init stage.
         * Reset it to zero to keep the same as JR1.
         */
        rv = bcm_port_class_set(unit, port1, bcmPortClassId, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set %d\n", rv);
            return rv;
        }

        rv = bcm_port_class_set(unit, port2, bcmPortClassId, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set %d\n", rv);
            return rv;
        }

        if (protection1) {
            rv = bcm_port_class_set(unit, second_port1, bcmPortClassId, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set %d\n", rv);
                return rv;
            }
        }
        if (protection2) {
            rv = bcm_port_class_set(unit, second_port2, bcmPortClassId, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_class_set %d\n", rv);
                return rv;
            }
        }
    }
    return rv;
}

int
run(int unit, int port1, int type1, int protection1, int second_port1 /* used if protection1 is set */,
    int port2, int type2, int protection2, int second_port2 /* used if protection2 is set */,
    int facility /* facility or path protection type */) {
    int port, i, failover_flags;
    int rv;
    int dbl_tag = 0;
    int vlan = 100;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    /* configure the port for double tag packets - this function relevant for termination mode 22-23 only */
    rv = vswitch_cross_connect_double_tag_port_configuration(unit, port1, protection1,second_port1,port2, protection2, second_port2, &dbl_tag);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_cross_connect_double_tag_port_configuration\n");
        return rv;
    }

    cross_connect_info_init(port1, second_port1, port2, second_port2);  

    /* check impossible combinations */
    if (type1 == 2 && type2 == 2) {
        printf("Error, 2 mpls ports cannot be cross connected\n");
        return BCM_E_PORT;
    }

    if ((type1 == 3) || (type1 == 4) || (type2 == 3) || (type2 == 4)) { /* mim */
        /* init mim on the device */
        rv = mim_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, mim_init\n");
            return rv;
        }
    }
    rv = bcm_vlan_gport_add(unit, vlan, port1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_gport_add\n");
        return rv;
    }

    /* When using new vlan translation mode, tpid and vlan actions and mapping must be configured manually */
    advanced_vlan_translation_mode = is_device_or_above(unit, JERICHO2) | soc_property_get(unit, "bcm886xx_vlan_translate_mode", 0);
    if (advanced_vlan_translation_mode && type1 == 1 || type2 == 1) {

        /** In JR2, we use the default TPID configurations for the test*/
        if (!is_device_or_above(unit, JERICHO2)) {
            port = cross_connect_info.port_1;
            for (i = 0 ; i < 2 ; i++) {
                port_tpid_init(port, 1, 1);
                rv = port_tpid_set(unit);
                if (rv != BCM_E_NONE) {
                    printf("Error, port_tpid_set with port_1\n");
                    print rv;
                    return rv;
                }
                port = cross_connect_info.port_2;
            }

            if (protection1) {
                port = cross_connect_info.port_3;
                port_tpid_init(port, 1, 1);
                rv = port_tpid_set(unit);
                if (rv != BCM_E_NONE) {
                    printf("Error, port_tpid_set with port_1\n");
                    print rv;
                    return rv;
                }
            } 

            if (protection2) {
                port = cross_connect_info.port_4;
                port_tpid_init(port, 1, 1);
                rv = port_tpid_set(unit);
                if (rv != BCM_E_NONE) {
                    printf("Error, port_tpid_set with port_1\n");
                    print rv;
                    return rv;
                }
            }
        }

        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    if (protection2 == 2) {
        printf("Error, 1+1 protection is supported only for fisrt port \n");
        return BCM_E_PARAM;
    }

    /* crate Failover-id*/
    if (protection1 == 1 || protection1 == 2) {
        if (facility) {
            if(protection1 == 2){
               printf("Error, cannot use facility protection with 1+1 protection \n");
               return BCM_E_PARAM;
            }
            cross_connect_info.failover_id_1 = BCM_FAILOVER_ID_LOCAL;
        }
        else {
            if (protection1 == 1) { /* 1:1 Protection */
                failover_flags = BCM_FAILOVER_FEC;
            } else {                /* 1+1 Protection */
                failover_flags = BCM_FAILOVER_INGRESS;
            }

            /* create failover_id for port 1 */
            rv = bcm_failover_create(unit, failover_flags, &cross_connect_info.failover_id_1);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create (failover_id_1)\n");
                print rv;
                return rv;
            }

            if (decoupled_mode && protection1 == 2) {
                /* create failover_id for second_port1 */
                rv = bcm_failover_create(unit, failover_flags, &cross_connect_info.failover_id_3);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_failover_create (failover_id_3)\n");
                    print rv;
                    return rv;
                }
            }

            if(verbose >= 1) {
                printf("created failover_id_1 0x0%8x\n", cross_connect_info.failover_id_1);
            }
        }
    }
    /* 1+1 protection: create MC group */
    if (protection1 == 2) {
        /* create MC for protection */
        egress_mc = 0;
        rv = multicast__open_mc_group(unit, &cross_connect_info.protection_mc1, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2));
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__open_mc_group\n");
            return rv;
        }
        if(verbose >= 1) {
            printf("created protection_mc1 : 0x0%8x\n", cross_connect_info.protection_mc1);
        }
    }
    else{
        cross_connect_info.protection_mc1 = 0;
    }


    if (protection2) {
        if (facility) {
            cross_connect_info.failover_id_2 = BCM_FAILOVER_ID_LOCAL;
        }
        else {
            /* create failover_id for port 2 */
            rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &cross_connect_info.failover_id_2);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create (failover_id_2)\n");
                print rv;
                return rv;
            }
            if(verbose >= 1) {
                printf("created failover_id_2 : 0x0%8x\n", cross_connect_info.failover_id_2);
            }

        }
    }
    /* 1+1 protection */
    else if (protection2 == 2) {
        /* create MC for protection */
        egress_mc = 0;
        rv = multicast__open_mc_group(unit, &cross_connect_info.protection_mc2, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2));
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__open_mc_group\n");
            return rv;
        }

        if(verbose >= 1) {
            printf("created protection_mc2 : 0x0%8x\n", cross_connect_info.protection_mc2);
        }

    }
    else {
        cross_connect_info.protection_mc2 = 0;
    }
        
    switch(type1) {
    case 1:
        /* create vlan port */
        rv = vlan_port_create(unit,
                              cross_connect_info.port_1, 
                              cross_connect_info.outer_vlan_base++, 
                              cross_connect_info.eg_vlan_base++, 
                              &(cross_connect_info.vlan_port_1), 
                              cross_connect_info.failover_id_1, 
                              0,
                              cross_connect_info.protection_mc1,
                              dbl_tag);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_1);
            return rv;
        }
        if(verbose >= 1) {
            printf("created vlan_port_1 : 0x0%8x with port = %d, vlan = %d\n",
                   cross_connect_info.vlan_port_1, cross_connect_info.port_1, cross_connect_info.outer_vlan_base - 1);
        }


        if (protection1 == 1 || protection1== 2) {
            /* create another vlan port - use first port as protection */

            if (!decoupled_mode) {
                rv = vlan_port_create(unit, 
                cross_connect_info.port_3, 
                cross_connect_info.outer_vlan_base++, 
                cross_connect_info.eg_vlan_base++, 
                &(cross_connect_info.vlan_port_3), 
                cross_connect_info.failover_id_1, 
                cross_connect_info.vlan_port_1,
                cross_connect_info.protection_mc1,
                dbl_tag);
            }
            else if (protection1 == 2){ /* decoupled mode */
                rv = vlan_port_create(unit, 
                cross_connect_info.port_3, 
                cross_connect_info.outer_vlan_base++, 
                cross_connect_info.eg_vlan_base++, 
                &(cross_connect_info.vlan_port_3), 
                cross_connect_info.failover_id_3, 
                0,
                cross_connect_info.protection_mc1,
                dbl_tag);
            }

            if (rv != BCM_E_NONE) {
                printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_3);
                return rv;
            }
            if(verbose >= 1) {
                printf("created vlan_port_3 (protection for 1) : 0x0%8x with port = %d, vlan = %d\n",
                   cross_connect_info.vlan_port_3, cross_connect_info.port_3, cross_connect_info.outer_vlan_base - 1);
            }
            cross_connect_info.gports.port1 = cross_connect_info.vlan_port_3;

            /* 1+1 protection */
            if(protection1 == 2) {
                cross_connect_info.gports.port1 = cross_connect_info.vlan_port_1;
                cross_connect_info.prim_gport_1 = cross_connect_info.vlan_port_3;

                /* add gport to protection MC */
                rv = multicast__vlan_port_add(unit,cross_connect_info.protection_mc1,cross_connect_info.port_1,cross_connect_info.vlan_port_1, 0);
                if (rv != BCM_E_NONE) {
                    printf("Error, in multicast__vlan_port_add with port %d\n", cross_connect_info.port_1);
                    return rv;
                }
                rv = multicast__vlan_port_add(unit,cross_connect_info.protection_mc1,cross_connect_info.port_3,cross_connect_info.vlan_port_3, 0);
                if (rv != BCM_E_NONE) {
                    printf("Error, in multicast__vlan_port_add with port %d\n", cross_connect_info.port_1);
                    return rv;
                }
            }
        }
        else {
            cross_connect_info.gports.port1 = cross_connect_info.vlan_port_1;
        }
        break;

    case 2:
        /* create mpls port */
        rv = mpls_port_create(unit, 
                              cross_connect_info.port_1, 
                              &(cross_connect_info.mpls_port_1), 
                              cross_connect_info.failover_id_1, 
                              0,
                              cross_connect_info.protection_mc1);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_port_create with port %d\n", cross_connect_info.port_1);
            return rv;
        }

        if (protection1) {
            /* create another mpls port - use first port as protection */
           if (!decoupled_mode) {
               rv = mpls_port_create(unit, 
                                     cross_connect_info.port_3, 
                                     &(cross_connect_info.mpls_port_2), 
                                     cross_connect_info.failover_id_1, 
                                     cross_connect_info.mpls_port_1.mpls_port_id,
                                     cross_connect_info.protection_mc1);
            }
            else if (protection1 == 2){ /* decoupled mode */
               rv = mpls_port_create(unit, 
                     cross_connect_info.port_3, 
                     &(cross_connect_info.mpls_port_2), 
                     cross_connect_info.failover_id_3, 
                     0,
                     cross_connect_info.protection_mc1);
            }
 
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_port_create with port %d\n", cross_connect_info.port_3);
                return rv;
            }

            cross_connect_info.gports.port1 = cross_connect_info.mpls_port_2.mpls_port_id;

            /* 1+1 protection */
            if(protection1 == 2) {
                int egr_mpls_port_id_1, egr_mpls_port_id_2;
                cross_connect_info.gports.port1 = cross_connect_info.mpls_port_1.mpls_port_id;
                cross_connect_info.prim_gport_1 = cross_connect_info.mpls_port_2.mpls_port_id;
                egr_mpls_port_id_1 = cross_connect_info.mpls_port_1.mpls_port_id;
                egr_mpls_port_id_2 = cross_connect_info.mpls_port_2.mpls_port_id;
                    
                if (is_device_or_above(unit, JERICHO2)) {
                    /** In JR2, egress_only flag is hoped in add egress mpls_port to mc group.*/
                    BCM_GPORT_SUB_TYPE_LIF_SET(egr_mpls_port_id_1, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, egr_mpls_port_id_1);
                    BCM_GPORT_MPLS_PORT_ID_SET(egr_mpls_port_id_1, egr_mpls_port_id_1);

                    BCM_GPORT_SUB_TYPE_LIF_SET(egr_mpls_port_id_2, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, egr_mpls_port_id_2);
                    BCM_GPORT_MPLS_PORT_ID_SET(egr_mpls_port_id_2, egr_mpls_port_id_2);
                }

                /* add gport to protection MC */
                rv = multicast__mpls_port_add(unit,cross_connect_info.protection_mc1,cross_connect_info.port_1,egr_mpls_port_id_1, 0/*is_egress*/);
                if (rv != BCM_E_NONE) {
                    printf("Error, in multicast_vpls_port_add with port %d\n", cross_connect_info.port_1);
                    return rv;
                }
                rv = multicast__mpls_port_add(unit,cross_connect_info.protection_mc1,cross_connect_info.port_3,egr_mpls_port_id_2, 0 /*is_egress*/);
                if (rv != BCM_E_NONE) {
                    printf("Error, in multicast_vpls_port_add with port %d\n", cross_connect_info.port_1);
                    return rv;
                }
            }
        }
        else {
            cross_connect_info.gports.port1 = cross_connect_info.mpls_port_1.mpls_port_id;
        }
        break;

    case 3:
        /* create mim port */
        rv = mim_port_create(unit, 
                             cross_connect_info.port_1, 
                             &(cross_connect_info.mim_port_1), 
                             &(cross_connect_info.bvid_1),
                             cross_connect_info.isid_base++, 
                             cross_connect_info.failover_id_1, 
                             0,
                             0);
        if (rv != BCM_E_NONE) {
            printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_1);
            return rv;
        }

        if (protection1) {
            /* create another mim port - use first port as protection */
            rv = mim_port_create(unit, 
                                 cross_connect_info.port_3, 
                                 &(cross_connect_info.mim_port_2), 
                                 &(cross_connect_info.bvid_2),
                                 cross_connect_info.isid_base++, 
                                 cross_connect_info.failover_id_1, 
                                 cross_connect_info.mim_port_1.mim_port_id,
                                 0);
            if (rv != BCM_E_NONE) {
                printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_3);
                return rv;
            }

            cross_connect_info.gports.port1 = cross_connect_info.mim_port_2.mim_port_id;
        }
        else {
            cross_connect_info.gports.port1 = cross_connect_info.mim_port_1.mim_port_id;
        }
        break;
    case 4:
        /* create mim port */
        rv = mim_port_create(unit, 
                             cross_connect_info.port_1, 
                             &(cross_connect_info.mim_port_1), 
                             &(cross_connect_info.bvid_1),
                             cross_connect_info.isid_base++, 
                             cross_connect_info.failover_id_1, 
                             0,
                             1);
        if (rv != BCM_E_NONE) {
            printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_1);
            return rv;
        }

        if (protection1) {
            /* create another mim port - use first port as protection */
            rv = mim_port_create(unit, 
                                 cross_connect_info.port_3, 
                                 &(cross_connect_info.mim_port_2), 
                                 &(cross_connect_info.bvid_2),
                                 cross_connect_info.isid_base++, 
                                 cross_connect_info.failover_id_1, 
                                 cross_connect_info.mim_port_1.mim_port_id,
                                 1);
            if (rv != BCM_E_NONE) {
                printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_3);
                return rv;
            }

            cross_connect_info.gports.port1 = cross_connect_info.mim_port_2.mim_port_id;
        }
        else {
            cross_connect_info.gports.port1 = cross_connect_info.mim_port_1.mim_port_id;
        }
        break;
    default:
        printf("Error, type1 cannot be <1 or >4\n");
        return BCM_E_PARAM;
    }

    switch(type2) {
    case 1:
        /* create vlan port */
        rv = vlan_port_create(unit,
                              cross_connect_info.port_2, 
                              cross_connect_info.outer_vlan_base++, 
                              cross_connect_info.eg_vlan_base++, 
                              &(cross_connect_info.vlan_port_2), 
                              cross_connect_info.failover_id_2, 
                              0,
                              cross_connect_info.protection_mc2,
                              dbl_tag);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_2);
            return rv;
        }
        if(verbose >= 1) {
            printf("created vlan_port_2 : 0x0%8x with port = %d, vlan = %d\n",
                   cross_connect_info.vlan_port_2, cross_connect_info.port_2, cross_connect_info.outer_vlan_base - 1);
        }


        if (protection2) {
            /* create another vlan port - use first port as protection */
           rv = vlan_port_create(unit, 
                                 cross_connect_info.port_4, 
                                 cross_connect_info.outer_vlan_base++, 
                                 cross_connect_info.eg_vlan_base++, 
                                 &(cross_connect_info.vlan_port_4), 
                                 cross_connect_info.failover_id_2, 
                                 cross_connect_info.vlan_port_2,
                                 cross_connect_info.protection_mc2,
                                 dbl_tag);
            if (rv != BCM_E_NONE) {
                printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_4);
                return rv;
            }

            if(verbose >= 1) {
                printf("created vlan_port_4 : 0x0%8x with port = %d, vlan = %d\n",
                       cross_connect_info.vlan_port_4, cross_connect_info.port_4, cross_connect_info.outer_vlan_base - 1);
            }

            cross_connect_info.gports.port2 = cross_connect_info.vlan_port_4;
        }
        else {
            cross_connect_info.gports.port2 = cross_connect_info.vlan_port_2;
        }
        break;

    case 2:
        /* create mpls port */
        rv = mpls_port_create(unit, 
                              cross_connect_info.port_2, 
                              &(cross_connect_info.mpls_port_1), 
                              cross_connect_info.failover_id_2, 
                              0,
                              cross_connect_info.protection_mc2);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_port_create with port %d\n", cross_connect_info.port_2);
            return rv;
        }

        if (protection2) {
            /* create another mpls port - use first port as protection */
           rv = mpls_port_create(unit, 
                                 cross_connect_info.port_4, 
                                 &(cross_connect_info.mpls_port_2), 
                                 cross_connect_info.failover_id_2, 
                                 cross_connect_info.mpls_port_1.mpls_port_id,
                                 cross_connect_info.protection_mc2);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_port_create with port %d\n", cross_connect_info.port_4);
                return rv;
            }

            cross_connect_info.gports.port2 = cross_connect_info.mpls_port_2.mpls_port_id;
        }
        else {
            cross_connect_info.gports.port2 = cross_connect_info.mpls_port_1.mpls_port_id;
        }
        break;

    case 3:
        /* create mim port */
        rv = mim_port_create(unit, 
                             cross_connect_info.port_2, 
                             &(cross_connect_info.mim_port_1),
                             &(cross_connect_info.bvid_1),
                             cross_connect_info.isid_base++,  
                             cross_connect_info.failover_id_2, 
                             0,
                             0);
        if (rv != BCM_E_NONE) {
            printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_2);
            return rv;
        }

        if (protection2) {
            /* create another mim port - use first port as protection */
            rv = mim_port_create(unit, 
                                 cross_connect_info.port_4, 
                                 &(cross_connect_info.mim_port_2),
                                 &(cross_connect_info.bvid_2),
                                 cross_connect_info.isid_base++, 
                                 cross_connect_info.failover_id_2, 
                                 cross_connect_info.mim_port_1.mim_port_id,
                                 0);
            if (rv != BCM_E_NONE) {
                printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_4);
                return rv;
            }

            cross_connect_info.gports.port2 = cross_connect_info.mim_port_2.mim_port_id;
        }
        else {
            cross_connect_info.gports.port2 = cross_connect_info.mim_port_1.mim_port_id;
        }
        break;
    case 4:
        /* create mim port */
        rv = mim_port_create(unit, 
                             cross_connect_info.port_2, 
                             &(cross_connect_info.mim_port_1),
                             &(cross_connect_info.bvid_1),
                             cross_connect_info.isid_base++,  
                             cross_connect_info.failover_id_2, 
                             0,
                             1);
        if (rv != BCM_E_NONE) {
            printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_2);
            return rv;
        }

        if (protection2) {
            /* create another mim port - use first port as protection */
            rv = mim_port_create(unit, 
                                 cross_connect_info.port_4, 
                                 &(cross_connect_info.mim_port_2),
                                 &(cross_connect_info.bvid_2),
                                 cross_connect_info.isid_base++, 
                                 cross_connect_info.failover_id_2, 
                                 cross_connect_info.mim_port_1.mim_port_id,
                                 1);
            if (rv != BCM_E_NONE) {
                printf("Error, in mim_port_create with port %d\n", cross_connect_info.port_4);
                return rv;
            }

            cross_connect_info.gports.port2 = cross_connect_info.mim_port_2.mim_port_id;
        }
        else {
            cross_connect_info.gports.port2 = cross_connect_info.mim_port_1.mim_port_id;
        }
        break;
    default:
        printf("Error, type2 cannot be <1 or >4\n");
        return BCM_E_PARAM;
    }

    /* cross connect the 2 ports */
    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x0%8x \n", cross_connect_info.gports.port1, cross_connect_info.gports.port2);
    }

    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_info.gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }

    /* 1+1 connect both protection & primary to another side */
    if (protection1 == 2) {

        bcm_vswitch_cross_connect_t gports2;
        bcm_vswitch_cross_connect_t_init(&gports2);
        gports2.port1 = cross_connect_info.prim_gport_1;
        gports2.port2 = cross_connect_info.gports.port2;

        if(verbose >= 1) {
            printf("connect port1:0x0%8x with port2:0x0%8x \n", gports2.port1, gports2.port2);
        }
        rv = bcm_vswitch_cross_connect_add(unit, &gports2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            print rv;
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * add gport of type mpls-port to the multicast
 */
int failover1_set(int unit, int enable){
    int rv;
    
    rv = bcm_failover_set(unit, cross_connect_info.failover_id_1, enable);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", cross_connect_info.failover_id_1);
        return rv;
    }
    
    return rv;
}

void
print_gport(bcm_gport_t gport) {
    if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        printf("MPLS port id = %d\n", BCM_GPORT_MPLS_PORT_ID_GET(gport));
    }
    else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        printf("MIM port id = %d\n", BCM_GPORT_MIM_PORT_ID_GET(gport));
    }
    else if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        printf("VLAN port id = %d\n", BCM_GPORT_VLAN_PORT_ID_GET(gport));
    }
    else {
        print gport;
    }
}

/* call back for traverse */
int
call_back(int unit,  bcm_vswitch_cross_connect_t *t, int* ud) {
    printf("traverse call back no. %d\n", (*ud)++);
    printf("port 1: ");
    print_gport(t->port1);
    printf("port 2: ");
    print_gport(t->port2);
    printf("\n");
    return BCM_E_NONE;
}

int
traverse_run(int unit) {

    int rv, traverse_no = 1;

    /* add another vlan port, that is not cross-connected to any port */
    rv = vlan_port_create(unit, 
                          cross_connect_info.port_5, 
                          cross_connect_info.outer_vlan_base++, 
                          cross_connect_info.eg_vlan_base++, 
                          &(cross_connect_info.vlan_port_2), 
                          0, 
                          0,
                          0);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_5);
        return rv;
    }

    /* traverse - print all the cross-connected ports */
    rv = bcm_vswitch_cross_connect_traverse(unit, call_back, &traverse_no);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_traverse\n");
        return rv;
    }

    return BCM_E_NONE;
}

int 
delete_all(int unit) {

    int rv, traverse_no = 1;

    rv = bcm_vswitch_cross_connect_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_delete_all\n");
        print rv;
        return rv;
    }

    rv = bcm_vswitch_cross_connect_traverse(unit, call_back, &traverse_no);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_traverse\n");
        return rv;
    }

    return BCM_E_NONE;
}

int get_vswitch_p2p_encap_id(){
    return vswitch_p2p_encap_id;
}
 

