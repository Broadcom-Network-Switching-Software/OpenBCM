/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * Cint cos marking example.
 *
 * Cross connect two ports (denoted @in_port and @out_port), and set @out_port to perform
 * DSCP/EXP marking when the expected traffic (vlan 10) is received from in_port.
 * Set @port to do DSCP marking for ip packets with Ethernet TPID (VLAN) = 10.
 * The packets are expected to be received on in_port.
 * The resulting packets will be sent from out_port.
 *
 * There are two types of calling sequences - basic and advanced.
 * One of the parameters for determining the DSCP/EXP of the new packets
 * is inlif profile.
 * The inlif profile is a property of the lif.
 * In basic mode, the inlif profile is avoided altogether by managing it internally.
 * In basic mode the user sets the port for DSCP/EXP marking and does not
 * need to do anything about the inlif profile.
 * In advanced mode the user manages inlif profiles and sets the required inlif
 * profile for each port.
 * Therefore when setting the DSCP/EXP mapping the advanced user has more control
 * since he/she can control the DSCP/EXP mapping by inlif profile as well as other
 * parameters.
 *
 * Calling sequence:
 *  1. Map drop precedence values to resolved drop precedence values.
 *     The DSCP/EXP marking is done according to resolved drop precedence values which are 1 bit.
 *     However the drop precedence of packets in the device is 2 bits.
 *     This map is used to resolve which 1 bit resolved drop precedence should be
 *     used for each 2 bit drop precedence value.
 *  2. [Basic Mode]
 *       Set the in port to do DSCP/EXP marking.
 *     [Advanced Mode]
 *       Set some in inlif profile to do DSCP/EXP marking and set the inlif profile of the
 *       in port to it.
 *  3. [Basic Mode]
 *       Create a QoS profile and map (tc,resolved-dp,qos_profile) to the desired
 *       EXP and DSCP.
 *     [Advanced Mode]
 *       Create a QoS profile and map (tc,resolved-dp,qos_profile,inlif_profile) to the desired
 *       EXP and DSCP.
 *  5. Set the QoS profile to the created QoS profile.
 *
 * Traffic:
 *   Send IP packets with VLAN=10 and any MAC Destination Address in the Ethernet header.
 *
 * When the expected packets are sent on in_port, the DSCP/EXP value is changed according to the setup.
 *
 * Run script:
 *
 * For simple mode, the following SOC property should be set:
 * bcm886xx_qos_l3_l2_marking=1
 * 
   cint cint_vswitch_cross_connect_p2p.c
   cint cint_qos_marking.c
   cint
   // print qos_map_dscp_marking(@unit,@in_port,@out_port);
   print qos_map_dscp_marking(unit,13,200);
 */

int verbose = 3;

struct qos_marking_info_s {
    /* In advanced mode, this inlif_profile will be used for the gport, and will be set for DSCP/EXP marking. */
    int inlif_profile;
    /* The cross connection will be between port pin with vlan vid and port pout and vlan vid+1. */
    int vid; 
    /* If non-zero then the cint uses the simple mode calling sequence. */
    /* Otherwise the advanced mode will be used. */
    int simple_mode;
    /* TC-DP mapping with one bit DP for Jericho/QMX B0 */
    int dp_map_mode;
    /* The IP DSCP will be set to this value for the expected traffic. */
    int new_dscp;
    /* The MPLS EXP will be set to this value for the expected traffic. */
    int new_exp;
    /* The id to use for bcm_qos_map_create. If -1, no id is used (it will be automatically allocated). */
    int qos_profile_id;
};

qos_marking_info_s default_qos_marking_info = {
    2,  /*inlif_profile*/
    10, /*vid*/
    1,  /*simple_mode*/
    0,  /*dp_map_mode*/
    5,  /*new_dscp*/
    2,  /*new_exp*/
    1,   /*qos_profile_id*/
};

/* output fec*/
int  out_fec;
/* pmf group*/
bcm_field_group_t ttl_group;
/*pmf action entry*/
bcm_field_entry_t ttl_entry;

static int dscp_marking_id = -1;
static int in_dscp_marking_id = -1;

int qos_map_dscp_marking_init(int unit, int simple_mode, int dp_map_mode)
{
	int rv = 0;
	
    default_qos_marking_info.simple_mode = simple_mode;
    default_qos_marking_info.dp_map_mode = dp_map_mode;
	
    return rv;
}


/*set in-ttl to 50 for inheritance*/
int qos_map_ttl_classification_set(int unit, bcm_port_t in_port)
{
    bcm_field_aset_t  aset;
    bcm_field_qset_t qset;
    int rv;

    BCM_FIELD_QSET_INIT(qset);          
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    rv = bcm_field_group_create(unit, qset, 2, &ttl_group);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create_id $rv\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTtlSet);
    rv = bcm_field_group_action_set(unit, ttl_group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set $rv\n");
        return rv;
    }
    
    rv = bcm_field_entry_create(unit, ttl_group, &ttl_entry);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit,ttl_entry,in_port, BCM_FIELD_EXACT_MATCH_MASK);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qualify_InPort failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_action_add(unit, ttl_entry, bcmFieldActionTtlSet, 50, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, ttl_group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    return rv;
}


int qos_map_dscp_ingress_profile(int unit)
{   
    bcm_qos_map_t l3_ing_map;
    int dscp, exp;
    int rv;
    
    /* create QoS ID (new cos profile) */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &in_dscp_marking_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_INGRESS\n");
        return rv;
    }
    printf("created QoS-id =0x%08x, \n", in_dscp_marking_id);
    
    /* set QoS mapping for L3 in ingress:
       map In-DSCP-EXP = IP-Header.TOS - 1 */
    for (dscp=0; dscp<256; dscp++) {
        bcm_qos_map_t_init(&l3_ing_map);
        l3_ing_map.dscp = dscp; /* packet DSCP (TOS) */
        l3_ing_map.int_pri = dscp % 8; /* TC */
        l3_ing_map.color = dscp % 2; /* DP */
        l3_ing_map.remark_int_pri = (dscp - 1) & 0xff; /* in-DSCP-exp */

        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &l3_ing_map, in_dscp_marking_id);
        if (rv != BCM_E_NONE) {
            printf("Error, set QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
            return rv;
        }
    }

    printf("set ingress QoS mapping for L3\n");
    
    return rv;
}

	

int qos_map_dscp_marking_profile(int unit, int simple, int dp_map)
{
    int rv = 0;
    int inlif_profile;
    int new_dscp;
    int new_exp;
    int tc_idx;
    int dp_idx;
    int qos_profile_id;
    int qos_create_flags;
    bcm_qos_map_t map;
    qos_marking_info_s info = default_qos_marking_info;

    inlif_profile = default_qos_marking_info.inlif_profile;
    new_dscp = default_qos_marking_info.new_dscp;
    new_exp = default_qos_marking_info.new_exp;
    qos_profile_id = default_qos_marking_info.qos_profile_id;

    qos_create_flags = BCM_QOS_MAP_L3_L2 | BCM_QOS_MAP_EGRESS;
    if (qos_profile_id != -1) {
        qos_create_flags |= BCM_QOS_MAP_WITH_ID;
        dscp_marking_id = qos_profile_id;
    }

    if (verbose >= 2) {
        printf("Creating qos profile...\n");
        if (qos_profile_id != -1) {
            printf("Using id %d for qos profile.\n", qos_profile_id);
        }
    }

    rv = bcm_qos_map_create(unit, qos_create_flags, &dscp_marking_id);
    if (rv) {
        printf("Error doing bcm_qos_map_create for flags BCM_QOS_MAP_L3_L2 | BCM_QOS_MAP_EGRESS.\n");
        print rv;
        return rv;
    }

    bcm_qos_map_t_init(&map);
    for (tc_idx = 0; tc_idx < 8; tc_idx++) {
        for (dp_idx = 0; dp_idx < 2; dp_idx++) {
            map.int_pri = tc_idx; /* TC */
            if (dp_map)
                map.remark_color = 0; /* Resolved-DP */
            else
                map.remark_color = dp_idx;
            if (!simple) {
                if (!dp_map) {
                    map.port_offset = inlif_profile; /* FHEI InLIF profile (2 bits, not 4) */
                } else {
                    map.port_offset = 0; /* Resolved-IN-LIF-Profiles. */
                }
            }
            map.dscp = new_dscp; /* DSCP */
            map.exp = new_exp; /* EXP */
            rv = bcm_qos_map_add(unit, 0, &map, dscp_marking_id);
            if (rv != BCM_E_NONE) {
                printf("Error doing bcm_qos_map_add(0,0,map=<tc=%d,resolved-dp=%d,", tc_idx, dp_idx);
                if (!simple) {
                    printf("inlif_profile=0x%x,", inlif_profile);
                }
                printf("dscp=%d,exp=%d>,map_id=%d).\n", new_dscp, new_exp, dscp_marking_id);
                print rv;
                return rv;
            }
        }
    }

    return rv;
}

int qos_map_dscp_marking_profile_get(int unit)
{
    return dscp_marking_id;
}

int qos_map_dscp_ingress_profile_get(int unit)
{
    return in_dscp_marking_id;
}

int qos_map_dscp_marking_port_enable(int unit, int port, int enable)
{
    int rv = 0;
    bcm_gport_t gport;

    /* Set DSCP/EXP marking for gport */
    if (verbose >= 2) {
        printf(" qos_map_dscp_marking_port_disable on port: %d, enable: %d.\n", port, enable);
    }

    BCM_GPORT_LOCAL_SET(gport,port);
    rv = bcm_port_control_set(unit, gport, bcmPortControlEgressModifyDscp, enable);
    if (rv != BCM_E_NONE) {
        printf("Error doing bcm_port_control_set(0,%d,bcmPortControlEgressModifyDscp,0).\n",gport);
        print rv;
        return rv;
    }

       return rv;
}

/** 
 * Synopsis:
 *  Configure DSCP/EXP marking on @in_port and cross connect it to @out_port
 *  (for IP/MPLS packets). 
 * 
 * Calling Sequence: 
 *  1. Cross connect (port=in_port, tpid=info.vid) to (port=out_port, tpid=info.vid+1).
 *  2. Set the resolved drop precedence value to 0 for all drop precedence values (0-3).
 *  3. Set the in_port for basic mode (or the inlif for advanced mode) to do DSCP/EXP marking:
 *     [Basic Mode]
 *       Set @in_port to perform DSCP/EXP marking.
 *     [Advanced Mode]
 *       Set the inlif profile of in_port to info.inlif_profile and enable DSCP/EXP marking for this
 *       profile. An inlif profile is a property of the inlif that can be used to set various parameters
 *       for the port such as the one described here (enable DSCP/EXP marking).
 *  4. Create a DSCP/EXP marking profile (denoted by qos_profile), and add the following mappings:
 *     [Basic Mode]
 *       (tc=[0..7], resolved-dp=0, qos_profile) -> (info.new_dscp, info.new_exp)
 *     [Advanced Mode]
 *       (tc=[0..7], resolved-dp=0, qos_profile, inlif_profile) -> (info.new_dscp, info.new_exp)     
 *  5. Set the quality of service profile of out_port to qos_profile (allocated with map_create).
 *  
 * Remarks: 
 *  - The global variable default_qos_marking_info is used for default test info.
 */
int qos_map_dscp_marking(int unit, int in_port, int out_port)
{

    int rv = 0;
    int is_jer_b0;
    int is_qmx_b0;
    int is_qax;
    int map_id;
    int in_vlan;
    int out_vlan;
    bcm_port_t gport_in;
    bcm_port_t gport_out;
    int simple;
    int dp_map;
    uint32 inlif_profile;
    qos_marking_info_s info = default_qos_marking_info;
    
    rv = is_jericho_b0_only(unit, &is_jer_b0);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_jericho_b0_only\n", rv);
        return rv;
    }
    rv = is_qumran_mx_b0_only(unit, &is_qmx_b0);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_mx_b0_only\n", rv);
        return rv;
    }
    rv = is_qumran_ax(unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_ax\n", rv);
        return rv;
    }

    in_vlan = info.vid;
    out_vlan = in_vlan+1; /* The value is based on the cross_connect function. */
    inlif_profile = info.inlif_profile;
    simple = info.simple_mode;
    dp_map = info.dp_map_mode;

    if ((dp_map != 0) && (!is_jer_b0) && (!is_qmx_b0) && (!is_qax)) {
        printf("TC-DP mapping with one bit DP is only support on Jericho/QMX B0\n");
        return BCM_E_PARAM;
    }

    /* Cross connect in_port,vlan 10 to out_port,vlan 11 */
    if (verbose >= 2) {
        printf("Doing port setup (cross connect port %d vlan %d to port %d vlan %d)...\n", in_port, in_vlan, out_port, out_vlan);
    }

    rv = cross_connect(unit, in_port, &gport_in, out_port, &gport_out, in_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error doing port setup.\n");
        print rv;
        return rv;
    }

    /* Set all DPs (0-3) to Resolved-DP 0. */
    if (verbose >= 2) {
        printf("Setting all DPs to resolved-DP 0.\n");
    }
    rv = bcm_switch_control_set(unit, bcmSwitchColorL3L2Marking, 0);
    if (rv) {
        printf("Error doing bcm_switch_control_set with bcmSwitchColorL3L2Marking.\n");
        print rv;
        return rv;
    }

    if (simple) {
        /* Set DSCP/EXP marking for gport */
        if (verbose >= 2) {
            printf(" Simple mode, Setting the gport 0x%x to do dscp/exp marking.\n", gport_in, inlif_profile);
        }

        rv = bcm_port_control_set(unit, gport_in, bcmPortControlEgressModifyDscp, 1);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set(0,0x%x,bcmPortControlEgressModifyDscp,1).\n", gport_in);
            print rv;
            return rv;
        }
        if (dp_map) {
            /* Set TC-DP mapping with one bit DP mode. */
            if (verbose >= 2) {
                printf("Setting TC-DP mapping with one bit DP mode.\n");
            }
            rv = bcm_switch_control_set(unit, bcmSwitchL3L2MarkingMode, 1);
            if (rv) {
                printf("Error doing bcm_switch_control_set with bcmSwitchL3L2MarkingMode.\n");
                print rv;
                return rv;
            }
           if (!is_qax) {
               /* Set all IN-LIF-Profiles (0-3) to Resolved-IN-LIF-Profiles 0. */
               if (verbose >= 2) {
                   printf("Setting all DPs to resolved-DP 0.\n");
               }
               rv = bcm_switch_control_set(unit, bcmSwitchClassL3L2Marking, 0);
               if (rv) {
                   printf("Error doing bcm_switch_control_set with bcmSwitchClassL3L2Marking.\n");
                   print rv;
                   return rv;
               }
           }
        }
    } else {
        /* Set DSCP/EXP marking for inlif profile, and set the inlif profile to the gport */
        if (verbose >= 2) {
            printf("Advance mode, Setting the inlif profile of gport 0x%x to inlif profile 2.\n", gport_in);
        }
        /*split horizon occupy the lowest bit in initialization, so set bcmPortClassFieldIngress 1 which means in_lif_profile 2*/
        rv = bcm_port_class_set(unit, gport_in, bcmPortClassFieldIngress, 1);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_class_set(0,0x%x,bcmPortClassFieldIngress,1).\n", gport_in);
            print rv;
            return rv;
        }

        if (verbose >= 2) {
            printf("Setting inlif profile %d to do DSCP/EXP marking.\n", inlif_profile);
        }

        rv = bcm_port_control_set(unit, inlif_profile, bcmPortControlEgressModifyDscp, 1);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set(0,%d,bcmPortControlEgressModifyDscp,1).\n", inlif_profile);
            print rv;
            return rv;
        }

        if (dp_map) {
            /* Set TC-DP mapping with one bit DP mode. */
            if (verbose >= 2) {
                printf("Setting TC-DP mapping with one bit DP mode.\n");
            }
            rv = bcm_switch_control_set(unit, bcmSwitchL3L2MarkingMode, 1);
            if (rv) {
                printf("Error doing bcm_switch_control_set with bcmSwitchL3L2MarkingMode.\n");
                print rv;
                return rv;
            }
            if (!is_qax) {
                /* Set all IN-LIF-Profiles (0-3) to Resolved-IN-LIF-Profiles 0. */
                if (verbose >= 2) {
                    printf("Setting all DPs to resolved-DP 0.\n");
                }
                rv = bcm_switch_control_set(unit, bcmSwitchClassL3L2Marking, 0);
                if (rv) {
                    printf("Error doing bcm_switch_control_set with bcmSwitchClassL3L2Marking.\n");
                    print rv;
                    return rv;
                }
            }
        }
    }

    rv =  qos_map_dscp_marking_profile(unit, simple, dp_map);
    if (rv) {
        return rv;
    }

    if (verbose >= 2) {
        printf("Setting the qos profile of port 0x%x to qos profile %d.\n", out_port, dscp_marking_id);
    }
    rv = bcm_qos_port_map_set(unit, out_port, -1, dscp_marking_id);
    if (rv) {
        printf("Error doing bcm_qos_port_map_set(0,port_out=%d,0,map_id=%d(qos_profile)).\n", out_port, dscp_marking_id);
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int qos_map_dscp_inheritance_enable(int unit,int inheritance_enable)
{
    int rv = 0;
    bcm_gport_t gport;

    BCM_GPORT_FORWARD_PORT_SET(gport, out_fec);
    if(inheritance_enable){
       rv = bcm_port_control_set(unit, gport, bcmPortControlAllowQosInheritance, 1);
       if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set bcmPortControlAllowQosInheritance true.\n");
            print rv;
            return rv;
         }
    } else {
        rv = bcm_port_control_set(unit, gport, bcmPortControlAllowQosInheritance, 0);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set bcmPortControlAllowQosInheritance false.\n");
            print rv;
            return rv;
        }
    }
    return rv;
}


int qos_map_ttl_inheritance_enable(int unit,int inheritance_enable)
{
    int rv = 0;
    bcm_gport_t gport;

    BCM_GPORT_FORWARD_PORT_SET(gport, out_fec);
    if(inheritance_enable){
       rv = bcm_port_control_set(unit, gport, bcmPortControlAllowTtlInheritance, 1);
       if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set bcmPortControlAllowQosInheritance true.\n");
            print rv;
            return rv;
         }
    } else {
        rv = bcm_port_control_set(unit, gport, bcmPortControlAllowTtlInheritance, 0);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set bcmPortControlAllowQosInheritance false.\n");
            print rv;
            return rv;
        }
    }
    return rv;
}


int qos_map_dscp_marking_routed(int unit, int in_port, int out_port)
{

    int rv = 0;
    int is_qux;
    int map_id;
    int in_vlan;
    int out_vlan;
    bcm_port_t gport_in;
    bcm_port_t gport_out;
    int fec;
    int host;
    int encap_id; 
    int l3_intf_id, ing_intf_out; /* in-rif and out-rif */
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    
    rv = is_qumran_ux_only(unit, &is_qux);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_mx_b0_only\n", rv);
        return rv;
    }

    in_vlan = 10;
    out_vlan = in_vlan+1; /* The value is based on the cross_connect function. */
  
    if (!is_qux) {
        printf("TC-DP mapping with one bit DP is only support on QUX\n");
        return BCM_E_PARAM;
    }
    /* Cross connect in_port,vlan 10 to out_port,vlan 11 */
    if (verbose >= 2) {
        printf("Doing port setup (cross connect port %d vlan %d to port %d vlan %d)...\n", in_port, in_vlan, out_port, out_vlan);
    }

    /* create ingress router interface */
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;
    
    intf.qos_map_valid = 1;
    intf.qos_map_id = in_dscp_marking_id;
    
    rv = l3__intf_rif__create(unit, &intf);
    l3_intf_id = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
    
    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);
    
    /* create egress router interface */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE) {
      printf("fail open vlan(%d)\n", out_vlan);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, out_vlan);
      return rv;
    }

    /* create egress object  */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
    l3eg.out_gport = out_port;
    l3eg.vlan = out_vlan;
    l3eg.fec_id = fec;
    l3eg.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    printf("created FEC-id =0x%08x, \n", fec);
    printf("next hop mac at encap-id %08x, \n", encap_id);

    /* add host point to FEC */
    host = 0x7fffff03;
    rv = add_host(unit, 0x7fffff03, 0, fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_port=%d, \n", in_port);
        return rv;
    }
    print_host("add entry ", host, 0);
    printf("---> egress-object=0x%08x, port=%d, \n", fec, out_port);

    out_fec = fec;

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = out_vlan;
    vlan_port.port = out_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.match_vlan        = out_vlan;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vlan_port_create index failed! %s\n", bcm_errmsg(rv));
    }
    gport_out = vlan_port.vlan_port_id;

    print gport_out;

    return BCM_E_NONE;
}

int qos_map_ttl_classification_cleanup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
  
    rv = bcm_field_entry_remove(unit, ttl_entry);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_remove returned $rv\n");
        return rv;
    }

    rv = bcm_field_entry_destroy(unit, ttl_entry);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_entry_destroy returned $rv\n");
        return rv;
    }
    
    rv = bcm_field_group_destroy(unit, ttl_group);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_destroy returned $rv\n");
        return rv;
    }

    return rv;
}
