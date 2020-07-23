/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
    This test demonstrates trill unicast ingress, egress and transit.
 
    It creates three trill ports where trill_port_local is the local Rbridge.
 
    Ingress: A l2_addr is added to forward the packet to trill_port_rb1.
        The ingress packet is encapsulated with an outer LL header defined in l3_if1, egr_obj_rb1.
        and a trill header: ing_nick=trill_port_local egr_nick=trill_port_rb1.
  
    Ttransit:
        In the trill header TTL is decreased, and all other fields are unchanged.  
 
    Egress: packet is decapsulated.

       Network diagram:
 
       ----------    vid_1 (host)
       | Host 1 | ---------
       ----------          |    ------------------
                           |    |    Local RB    |
       ----------          |    |                |
       | RB1    | --------------| port-1         |
       ----------          |    |                |
                           |    |                |
       ----------          |    ------------------
       | RB2    |----------|
       ----------    vid_2_desig (designated vlan)


    Ingress scenario: 
        Send: 
            eth : c_dmac, c_smac, vid1
        Recv: 
            eth:    remote_mac_rb1, vid_2
            trill:  trill_ethertype, hopcount, nickname_rb1, nickname_rb_local
            eth:    c_dmac, c_smac, vid1
  
    Transit scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, hopcount,nickname_rb2, nickname_rb1
            eth:    c_smac, c_dmac, vid1
        Recv:
            eth:    remote_mac_rb2,
            trill:  0x2109, hopcount-1,nickname_rb2, nickname_rb1
            eth:    c_smac, c_dmac, vid1        
     
    Egress :
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, hopcount,nickname_rb_local, nickname_rb1
            eth:    c_smac, c_dmac, vid1
        Recv:
            eth:    c_smac, c_dmac, vid1

    ****************************************************************************
    **                          Trill new bridge model                        **
    ****************************************************************************
    Ingress scenario: 
        Send: 
            eth : c_dmac, c_smac, vid1
        Recv: 
            eth:    remote_mac_rb1, vid_2
            trill:  trill_ethertype, hopcount, nickname_rb1, nickname_rb_local
            eth:    c_dmac, c_smac, vid1

    Work flow:
    - LEM lookup with the DA + VLAN to find the FEC pointer
    - FEC pointer point to the Destnation port and Trill-EEP
    - Trill-EEP build the Trill header and point to LL-EEP
    - LL-EEP build the DA, SA and VLAN.
    ---------------------------------------------------------------------------
    |    Table    |            KEY                |           Result          |
    ---------------------------------------------------------------------------
    |    LEM      |         DA + VLAN             |           FEC-Ptr         |
    ---------------------------------------------------------------------------
    |    FEC      |            FEC-Ptr            | Dest Port + Trill-EEP Ptr | 
    ---------------------------------------------------------------------------
    |    EEDB     |       Trill-EEP Ptr           | Egress Trill + LL-EEP Ptr |
    ---------------------------------------------------------------------------
    |    EEDB     |          LL-EEP Ptr           |       LL-DA + LL-VLAN     |
    ---------------------------------------------------------------------------
    
  
    Transit scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, hopcount,nickname_rb2, nickname_rb1
            eth:    c_smac, c_dmac, vid1
        Recv:
            eth:    remote_mac_rb2,
            trill:  0x2109, hopcount-1,nickname_rb2, nickname_rb1
            eth:    c_smac, c_dmac, vid1   
    
    Work flow:
    - Check outer DA == my_mac 
    - Check egress nickname = my_nickname
    - LEM adjacent check with the LL-SA + Port to Adjacent-EEP
    - LEM lookup with the egress nickname to find the LL-EEP.
    - LL-EEP build the DA, SA and VLAN.
    ---------------------------------------------------------------------------
    |    Table    |            KEY                |           Result          |
    ---------------------------------------------------------------------------
    |    LEM      |         SA + Port             |       Adjacent EEP        |
    ---------------------------------------------------------------------------
    |    LEM      |       Egress nickname         |            LL-EEP Ptr     | 
    ---------------------------------------------------------------------------
    |    EEDB     |          LL-EEP Ptr           |       LL-DA + LL-VLAN     |
    ---------------------------------------------------------------------------  
     
    Egress :
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, hopcount,nickname_rb_local, nickname_rb1
            eth:    c_smac, c_dmac, vid1
        Recv:
            eth:    c_smac, c_dmac, vid1
    Work flow:
    - Check outer DA == my_mac 
    - Check egress nickname = my_nickname
    - LEM adjacent check with the LL-SA + Port to Adjacent-EEP
    - LEM lookup with the egress nickname to find the LL-EEP.
    - LL-EEP build the DA, SA and VLAN.
    ---------------------------------------------------------------------------
    |    Table    |            KEY                |           Result          |
    ---------------------------------------------------------------------------
    |    LEM      |         DA + VLAN             |        Egress Port        |
    ---------------------------------------------------------------------------
    Configuration:
    - soc properties:
        bcm886xx_auxiliary_table_mode=1 (for arad/+ devices only)
        trill_mode=2
    - sequence:
        cint src/examples/dpp/utility/cint_utils_vlan.c
        cint src/examples/dpp/cint_trill_advanced_vlan_translation_mode.c
        cint src/examples/dpp/cint_advanced_vlan_translation_mode.c
        cint src/examples/dpp/cint_pmf_trill_2pass_snoop.c
        cint src/examples/dpp/cint_pmf_2pass_snoop.c
        cint src/examples/dpp/cint_trill.c
        cint src/examples/dpp/cint_port_tpid.c
        cint 
    Trill new bridge model unicast FGL:
        print trill_mode_set($unit, $mode);
        print test_trill_uni_fgl($unit,$inP,$outP,0xDDDD,$hop,1);
    Trill new bridge model multicast fecless FGL:
        print trill_mode_set($unit, $mode);
        test_trill_mc_transit_fecless_fgl($unit,$inP,$outP,0xDDDD,$hop,$group,1,0);
*/ 

int trill_bridge_mode = 0; /* 0: normal model, 1: trill new bridge model*/

bcm_mac_t  trill_vsi_mac_2    = {0x00, 0x00, 0x00, 0x00, 0x52, 0x26};
bcm_mac_t  remote_mac_rb1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
bcm_mac_t  remote_mac_rb2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
bcm_mac_t  c_dmac         = {0x00, 0x00, 0x00, 0x00, 0xcc, 0x33};
bcm_mac_t  c_smac         = {0x00, 0x00, 0x00, 0x00, 0xcc, 0x44};

bcm_mac_t  host1_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x14};
bcm_mac_t  host2_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
bcm_mac_t  host3_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
bcm_mac_t  host4_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x88};

bcm_vlan_t  vid_1       = 100;
bcm_vlan_t  vid_2_desig = 200;

int     nickname_rb_local  = 0x1234;
int     nickname_rb1  = 0xAA11;
int     nickname_rb2  = 0xAA22;

bcm_gport_t local_id = -1;
bcm_gport_t rb1_id = -1;
bcm_gport_t rb2_id = -1;
bcm_trill_port_t trill_port_local;
bcm_trill_port_t
        trill_port_rb1,
        trill_port_rb2;

int     trill_vlan_domain = 10;

int     port_1     = 13;
int     port_2     = 14;
int     port_3     = 15;
int     port_4     = 16;
int     recycle_port[2] = {40, 41};

int     hopcount = 12;

int     trill_ethertype = 0x22f3;
bcm_multicast_t l2mc_group = 9162;
uint32  dist_tree_1 = 0xBB11;
uint32  dist_tree_2 = 0xBB22;
uint32  dist_tree_3 = 0xBB33;

bcm_trill_port_t    dist_tree_1_port,
                    dist_tree_2_port,
                    dist_tree_3_port;

int trill_mc_group_num = 2;
bcm_multicast_t trill_mc_group = 6462;
bcm_trill_port_t    dist_tree_1_port_nb[2],
                    dist_tree_2_port_nb[2],
                    dist_tree_3_port_nb[2];


uint32   ecmp_tree = 0xBB44;
bcm_multicast_t dummy_mc_group = 6163;

uint8  no_mac[6]          = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

bcm_multicast_t     flood_mc_group = 6300;
int                 flood_nick     = 0xcc11;
int                 flood_fec      = 1350;
int                 flood_egr_if;
bcm_trill_port_t    flood_trill_port;
bcm_if_t            l3_intf_id = 0; 
 
const int bridge_to_bridge_encap_id = 0xffff;
const int trill_to_bridge_encap_id = 0xfffc;
 
int is_traps_set = FALSE;
int trill_uni_configured = FALSE;
int trill_uni_configured_fgl = FALSE;

int         trap_id[10];
int         trap_id_cnt=0;
int         trap_id_same_interface;
uint16  fgl_tpid = 0x893B; 
uint16  fgl_outer_tpid = fgl_tpid; 
uint16  fgl_inner_tpid = fgl_tpid; 
int     FGL_outer_vid = 50;
int     FGL_inner_vid = 150;

bcm_field_group_t       snoop_group;
bcm_gport_t             snoop_trap_gport_id[2] = { -1, -1};
bcm_gport_t             adj_trap;


const int           ecmp_mc_ing_nof_ifs = 3;
bcm_multicast_t     ecmp_mc_ing_mc_groups[ecmp_mc_ing_nof_ifs] = {6200,6201,6202};
bcm_trill_port_t    ecmp_mc_trill_ports[ecmp_mc_ing_nof_ifs];
bcm_l3_egress_ecmp_t ecmp;
bcm_if_t            ecmp_mpintf;
bcm_trill_port_t    ecmp_trill_port;

/* old implementation that configure sw db (mc-id to nickname)*/
int mc_id = 0;
/* old implementation creates ecmp port */
int create_ecmp_port = 0;
/* flag for the same interface filter */
int is_same_interface_filter_enabled = FALSE;

/* flag for the wide lif */
int is_wide_lif = FALSE;

struct cint_trill_cfg_s {
    int ecmp_api_is_ecmp;
};

cint_trill_cfg_s cint_trill_cfg = {
    1 /* ecmp_api_is_ecmp */
};

int trill_mode_set(int unit, int mode) {
	int rv = BCM_E_NONE;
	
    trill_bridge_mode = mode;
	
	return rv;
}

/* convert from trill gport to l3 intf object
 * gport and l3_if encoding: 
 * egress trill gport format:  {gport type[31:26] gport_sub_type[25:22] out-lif[17:0]}
 * l3_if:                      { type[32:29]  value[28:0]}
 * 
 * Conversion:
 * l3_if.type: encap_type, l3_if.value: gport.out-lif 
 */
int 
trill_gport_to_l3_intf(bcm_gport_t egress_trill_gport,  bcm_if_t* l3_if) {
    int out_lif_mask = 0x3FFFF; 
    int itf_encap = 0x2; 
    int itf_encap_shift = 29; 
    /* l3_if.type: encap_type, l3_if.value: gport.out-lif */
    *l3_if = (egress_trill_gport & out_lif_mask) | (itf_encap << itf_encap_shift); 
}

/* convert from trill gport to l3 intf object
 * gport and l3_if encoding: 
 * l3_if:                      { type[32:29]  value[28:0]}
 * egress trill gport format:  {gport type[31:26] gport_sub_type[25:22] out-lif[17:0]}
 * 
 * Conversion:
 * l3_if.type: encap_type, l3_if.value: gport.out-lif 
 */
int 
l3_intf_to_trill_gport(bcm_if_t l3_if, bcm_gport_t *egress_trill_gport) {
    int gport_encap = 0x2; 
    int gport_encap_shift = 22; 

    BCM_GPORT_TRILL_PORT_ID_SET(*egress_trill_gport, l3_if);
    *egress_trill_gport = *egress_trill_gport | (gport_encap << gport_encap_shift); 
}

int global_config(int unit) {
    int rv = 0;

  
    /* Set global hop count */
    rv = bcm_switch_control_set(unit, bcmSwitchTrillHopCount , hopcount);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_switch_control_set for bcmSwitchTrillHopCount\n");
        return rv;
    }
    
    /* Set Trill EtherType */
    rv = bcm_switch_control_set(unit, bcmSwitchTrillEthertype , trill_ethertype);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_switch_control_set for bcmSwitchTrillHopCount\n");
        return rv;
    }
    
    /* Set Trill-related traps */
    rv = traps_set(unit);
    BCM_IF_ERROR_RETURN(rv);   
    
    /* using PMF, set the In-LIF to be equal to the EEP (1st LEM lookup result. 1st LEM lookup is adjacency check) */
    rv = trill_in_lif_field_group_set(unit, 5, 6, 5, 6, adj_trap);
    BCM_IF_ERROR_RETURN(rv);
    
    /*
    rv = set_same_interface_filter(unit,port_1,0);
    return rv;*/
    return rv;
}

bcm_if_t get_encap_id_to_bridge (int unit ) {

    if (is_device_or_above(unit, JERICHO)) {
        return 0x3ffff;
    } else {
        return 0xffff;
    }
}

int port_config(int unit, int port, int designated_vlan) {
    int rv = 0;
	int is_arad_a0;
    int trill_transparent_service;
    /* Configure TPIDs */
    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    trill_transparent_service = soc_property_get(unit, "trill_transparent_service", 1);


    if (advanced_vlan_translation_mode ) {
        /* In advanced vlan translation mode, a longer process is needed to enable vlan editing (with tpid as key).
           View cint_port_tpid.c for details.*/
        rv = port_outer_tpid_set(unit, port, 0x8100, 0, 0);
        if (rv != BCM_E_NONE) {
           printf("Error, in port_outer_tpid_set 0x8100 port=%d\n", port);
           return rv;
        }
    } else {
        rv = bcm_port_tpid_add(unit, port , 0x8100, 0);
        if (rv != BCM_E_NONE) {
           printf("Error, in bcm_port_tpid_set 0x8100 port=%d\n", port);
           return rv;
        }
    }

    /* Enable trill per port (per port configuration related to adjacency check) */
    rv = bcm_port_control_set(unit, port, bcmPortControlTrill, 0x1);
    BCM_IF_ERROR_RETURN(rv);
    
    /* Allow only tagged packets on port when trill transparent service is not enabled. By default, in advanced vlan translation mode all packets are allowed.
       Discarding untagged packets in advanced vlan translation mode can be done by using bcm_port_tpid_class_set */
    if ((!advanced_vlan_translation_mode) && (!trill_transparent_service)) {
        rv = bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_UNTAG);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_discard_set\n");
            return rv;
        }
    }
    
    /* Map port to vlan domain */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, trill_vlan_domain);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set \n");
        return rv;
    }
    if (verbose >=2) {
        printf("port %d is mapped to vlan domain %d\n", port, trill_vlan_domain); 
    }

    /* Enable IGMP traps per port */
    rv = bcm_switch_control_port_set(unit, port, bcmSwitchIgmpUnknownDrop, 0x1);
    BCM_IF_ERROR_RETURN(rv);

	rv = is_arad_a0_only(unit, &is_arad_a0);
    if (is_arad_a0) {
        /* Disable filter same-interface */
        rv = bcm_port_control_set(unit, port, bcmPortControlBridge, 0x0);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Configure designated vlan on port 1 */
    rv = bcm_port_control_set( unit, port, bcmPortControlTrillDesignatedVlan, designated_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set: port=%d, vid=%d\n",port,designated_vlan);
        return rv;
    }

    return 0;
}





int port_config_reset(int unit, int port, int designated_vlan) {
    int rv = 0;
   /* Reset TPIDs */
    rv = bcm_port_tpid_delete_all(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all\n");
        return rv;
    }
    /* Reset designated vlan on port*/
    rv = bcm_port_control_set( unit, port, bcmPortControlTrillDesignatedVlan, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set: port=%d, vid=%d\n",port,vid_2_desig);
        return rv;
    }

    /* Disable trill per port (per port configuration related to adjacency check) */
    rv = bcm_port_control_set(unit, port, bcmPortControlTrill, 0x0);
    BCM_IF_ERROR_RETURN(rv);
    
    /* Allow all packets on port. Not neccassary in advanced vlan translation mode.*/
    if (!advanced_vlan_translation_mode) {
        rv = bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_discard_set\n");
            return rv;
        }
    }

    /* Reset port to vlan domain */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set \n");
        return rv;
    }

    /* Disable IGMP traps per port */
    rv = bcm_switch_control_port_set(unit, port, bcmSwitchIgmpUnknownDrop, 0);
    BCM_IF_ERROR_RETURN(rv);

    return 0;
}


/* Configure TPIDs for fine-grained*/
int port_config_FGL(int unit, int port) {
    int rv = 0;

    if (advanced_vlan_translation_mode) {
        /* In advanced vlan translation mode, a longer process is needed to enable vlan editing (with tpid as key).
           View cint_port_tpid.c for details.*/
        rv = port_dt_tpid_set(unit, port, 0x8100, 0x893B, 0, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in port_dt_tpid_set: port=%d\n", port);
            return rv;
        }    
    } else {
        rv = bcm_port_tpid_delete_all(unit,port); 
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_delete_all: port=%d\n", port);
            return rv;
        }    
        rv = bcm_port_tpid_add(unit,port,0x8100,0); 
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add: port=%d\n", port);
            return rv;
        }    
        rv = bcm_port_inner_tpid_set(unit,port,fgl_inner_tpid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set: port=%d\n", port);
            return rv;
        } 
        rv = bcm_port_tpid_add(unit,port,fgl_inner_tpid,0); 
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add: port=%d\n", port);
            return rv;
        } 
        printf("Configured FGL TPID for port=%d\n", port);
    }
    return 0;   
}
int vlan_init(int unit, bcm_vlan_t vid) {
    int rv = 0;
    
    /* Create vlan (vlan = VSI. Smaller than 4K) */
    rv = bcm_vlan_create(unit,vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        return rv;
    }
    

    return rv;
}
int vlan_term(int unit, bcm_vlan_t vid) {
    int rv = 0;
    
    /* delete vlan (vlan = VSI. Smaller than 4K) */
    rv = bcm_vlan_destroy(unit,vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy\n");
        return rv;
    }
    printf("Deleted vlan %d\n", vid);
    
    return rv;
}
int vlan_port_init(int unit, bcm_vlan_t vid, int port) {
    int rv = 0;
    bcm_pbmp_t pbmp, upbmp;
           
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
       
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }
    
    return rv;
}
int vlan_port_term(int unit, bcm_vlan_t vid, int port) {
    int rv = 0;
    bcm_pbmp_t pbmp, upbmp;
           
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
       
    rv = bcm_vlan_port_remove(unit, vid, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_delete unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }
    printf("Vlan port removed , vid %d, port %d\n", vid, port);
    
    return rv;
}
int
vlan_port_set(int unit, int port, bcm_vlan_t vid, bcm_vlan_t vsi) {
    int rv = 0;
    bcm_vlan_port_t  port_vlan;
    int trill_mode, is_fgl = 0;

    trill_mode = soc_property_get(unit , "trill_mode",0);
    is_fgl = (trill_mode == 2);

    
    /* Add a <VD,vid> -> lif mapping
       port_vlan.vlan_port_id is filled with a gport, which is a SW handle to the lif.
       HW constraint in ARAD: lif must be equal to vsi. Use ENCAP_WITH_ID,
       with encap_id = vsi to force this. */
    bcm_vlan_port_t_init(port_vlan);
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    port_vlan.match_vlan = vid;
    port_vlan.port = port;
    port_vlan.egress_vlan = vid;
    port_vlan.vsi = vsi;
    if (is_fgl) {
        port_vlan.flags = BCM_VLAN_PORT_INNER_VLAN_PRESERVE|BCM_VLAN_PORT_OUTER_VLAN_PRESERVE;
    } else {
        port_vlan.flags = BCM_VLAN_PORT_ENCAP_WITH_ID|BCM_VLAN_PORT_INNER_VLAN_PRESERVE|BCM_VLAN_PORT_OUTER_VLAN_PRESERVE;
        port_vlan.encap_id = vsi;
    }

    rv = bcm_vlan_port_create(unit,&port_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create\n");
        print port_vlan;
        return rv;
    }
    if (verbose >=2) {
        printf("port <%d, %d> -> lif: %d \n", port, vid, vsi); 
    }

  
    /* Map LIF -> VSI */
    rv = bcm_vswitch_port_add(unit, vsi, port_vlan.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add\n");
        return rv;
    }

    if (verbose >=2) {
        printf("map lif %d to vsi %d \n", port_vlan.vlan_port_id, vsi); 
    }

    
    if (advanced_vlan_translation_mode) { 
        rv = trill_advanced_vlan_translation_map_local_vsi_to_global_vsi(unit, vsi, port_vlan.egress_vlan, port_vlan.vlan_port_id, trill_mode);
        if (rv != BCM_E_NONE) {
            printf("Error, in trill_advanced_vlan_translation_map_local_vsi_to_global_vsi\n");
            print port_vlan;
            return rv;
        }
    }

    return rv;
}


int
vlan_port_unset(int unit, int port, bcm_vlan_t vid, bcm_vlan_t vsi) {
    int rv = 0;
    bcm_vlan_port_t  port_vlan;
    
    bcm_vlan_port_t_init(port_vlan);
    port_vlan.match_vlan = vid;
    port_vlan.port = port;
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;

    rv =  bcm_vlan_port_find(unit, &port_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_find\n");
        print port_vlan;
        return rv;
    }
    /* Unmap LIF -> VSI */
    rv = bcm_vswitch_port_delete(unit, vsi, port_vlan.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_delete\n");
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit,port_vlan.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy\n");
        print port_vlan;
        return rv;
    }
  

    
    return rv;
}

int
l3_intf_create(int unit, bcm_mac_t local_mac, bcm_vlan_t vsi, bcm_if_t *if_id) {
   int rv = 0;
   bcm_l3_intf_t l3_if;
    
    /* Create L3 interfaces for TRILL:
       Configures MyMac 
       TRILL_ONLY flag indicates not to allocate a RIF for this L3 interface
         (we take the OutVSI from the LL EEDB entry, and not outRif). */
    bcm_l3_intf_t_init(&l3_if);
    sal_memcpy(l3_if.l3a_mac_addr, local_mac, 6);
    l3_if.l3a_vid      = vsi;
    l3_if.l3a_flags    = BCM_L3_TRILL_ONLY;    

    rv = bcm_l3_intf_create(unit, &l3_if);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_intf_create, \n");
        print l3_if;
        return rv;
    }
    *if_id = l3_if.l3a_intf_id;
    return rv;
}
int
l3_intf_delete(int unit, bcm_if_t if_id) {
   int rv = 0;
   bcm_l3_intf_t l3_if;
    
    bcm_l3_intf_t_init(&l3_if);
    l3_if.l3a_intf_id      = if_id;

    rv = bcm_l3_intf_delete(unit, &l3_if);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_intf_delete, \n");
        print l3_if;
        return rv;
    }
    printf("Removed l3 intterface %d \n", if_id);
    
    return rv;
}

int
add_remote_rbridge(int unit, bcm_mac_t remote_mac_rb, bcm_vlan_t vsi, int port, int nickname, int flags, bcm_trill_port_t *trill_port) {
    int rv = 0;
    /* Used for old bridge model */
    create_l3_egress_s egr_obj_rb;
    bcm_if_t egr_if_id_rb;
    /* Used for new bridge model */
    create_l3_egress_s ll_eep;
    bcm_trill_port_t trill_eep;
    create_l3_egress_s trill_fec;
    bcm_if_t trill_fec_if_id;


    if (!trill_bridge_mode) {
        /* Add TRILL Egress Next Hop:
           - Allocates an EEDB entry, and configures MAC + Out-VSI
           - Allocates a FEC, and point it to the allocated EEDB entry. */
        sal_memcpy(egr_obj_rb.next_hop_mac_addr, remote_mac_rb, 6);
        egr_obj_rb.vlan     = vsi; /* VSI < 4K */
        egr_obj_rb.out_gport     = port;
        egr_obj_rb.l3_flags    = BCM_L3_TRILL_ONLY;

        rv = l3__egress__create(unit,&egr_obj_rb);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress__create, \n");
            return rv;
        }
        egr_if_id_rb = egr_obj_rb.fec_id;
        printf("Created egress obj: %d \n", egr_if_id_rb);

       /* Add TRILL Virtual Port for RB1:
          - Binds (SW) nickname and egress_if (FEC)
          - Adds adjacent mac -> eep mapping (taken from egr_if)
          - Added LEM entry: nickname -> FEC                   )
          - For Jericho: Configure orientation for inLif */
        bcm_trill_port_t_init(trill_port);
        trill_port->egress_if  =  egr_if_id_rb;
        trill_port->name       =  nickname;
        /* configure inLif orientation for Jericho */
        if (is_device_or_above(unit, JERICHO)) {
            trill_port->flags      |= BCM_TRILL_PORT_NETWORK; 
        }
        if (is_wide_lif) {
            trill_port->flags      |= BCM_TRILL_PORT_INGRESS_WIDE;
        }
    } else {
       /* Add remote RBridge
         * - build the LL-EEP for LL header
         * - build the Trill-EEP for trill header     
         * - return trill FEC */

        /* Add LL-EEP:
           - Allocates an EEDB entry, and configures MAC + Out-VSI. */
        sal_memcpy(ll_eep.next_hop_mac_addr, remote_mac_rb, 6);
        ll_eep.vlan     = vsi; /* VSI < 4K */

        rv = l3__egress_only_encap__create(unit,&ll_eep);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress_only_encap__create, \n");
            return rv;
        }
        printf("Created LL-EEP: 0x%x \n", ll_eep.arp_encap_id);

        /* Add Trill-EEP:
          - Allocates an EEDB entry, and configures Trill header, Next-pointer LL-EEP. */
        bcm_trill_port_t_init(&trill_eep);
        trill_eep.egress_if  =  ll_eep.arp_encap_id;
        trill_eep.virtual_name = nickname_rb_local;
        trill_eep.name       =  nickname;
        trill_eep.flags      |= BCM_TRILL_PORT_EGRESS;     
      
        rv = bcm_trill_port_add(unit, &trill_eep);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_trill_port_add, \n");
            return rv;
        }
        printf("Created Trill-EEP: 0x%x \n", trill_eep.trill_port_id);    
        
       /* Build Trill FEC:
           - Allocates a FEC, and point it to the allocated EEDB entry. */
        trill_gport_to_l3_intf(trill_eep.trill_port_id, &(trill_fec.out_tunnel_or_rif)); /* fec point to trill eedb */
        trill_fec.out_gport     = port;

        rv = l3__egress_only_fec__create(unit, &trill_fec);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress_only_fec__create, \n");
            return rv;
        }
        trill_fec_if_id = trill_fec.fec_id;
        printf("Created Trill FEC: 0x%x \n", trill_fec_if_id);

       /* Add Egress Trill packets:
          - Add SEM entry -> Learning-information.
          - For Jericho: Configure orientation for inLif */
        bcm_trill_port_t_init(trill_port);
        trill_port->egress_if  =  trill_fec_if_id;
        trill_port->name       =  nickname;
        trill_port->flags      |= BCM_TRILL_PORT_INGRESS | BCM_TRILL_PORT_TUNNEL; 
        /* configure inLif orientation for Jericho */
        if (is_device_or_above(unit, JERICHO)) {
            trill_port->flags      |= BCM_TRILL_PORT_NETWORK; 
        }
        if (is_wide_lif) {
            trill_port->flags      |= BCM_TRILL_PORT_INGRESS_WIDE;
        }
    }
   
    rv = bcm_trill_port_add(unit, trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_add, \n");
        return rv;
    }
    printf("Created trill_port: 0x%x \n", trill_port->trill_port_id);

    /* configure outlif orientation (LL outlif) */
    if (is_device_or_above(unit, JERICHO)) {
        bcm_gport_t tunnel_gport; 
        int frwrd_group; 
        /* convert l3_if encap object to tunnel gport */
        BCM_GPORT_TUNNEL_ID_SET(tunnel_gport, trill_bridge_mode ? ll_eep.arp_encap_id : egr_obj_rb.arp_encap_id); 

        printf("tunnel gport for LL: 0x%x \n", tunnel_gport);

        /* set network orientation for LL outlif */
        frwrd_group = 1; 
        rv = bcm_port_class_set(unit, tunnel_gport, bcmPortClassForwardEgress, frwrd_group); 
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_class_set \n");
            return rv;
        }
        printf("outlif orientation: %d updated for trill_port: 0x%x \n", frwrd_group, trill_port->trill_port_id);
    }
   /* add trill multicast adjacency entry */
   rv = mult_adjacency(unit, trill_port->trill_port_id, TRUE);

   if (rv != BCM_E_NONE) {
        printf("Error, in add mult_adjacency \n");
        return rv;
    }
    printf("Created multicast adjacency: %d \n", trill_port->trill_port_id);


    return rv;
}
int
remove_remote_rbridge(int unit, bcm_gport_t port_id) {
    int rv = 0;
    bcm_trill_port_t trill_port;
    bcm_l3_egress_t ll_eep;
    bcm_if_t ll_eep_if_id;
    bcm_trill_port_t trill_eep;
    bcm_l3_egress_t trill_fec;
    bcm_if_t trill_fec_if_id;


    printf("remove_remote_rbridge %d\n", port_id);
    if (port_id <= 0) {
        printf("Error, in remove_remote_rbridge illegal id %d\n", port_id);
        return rv;
    }

    /* delete trill multicast adjacency entry */
    rv = mult_adjacency(unit, port_id, FALSE);    
    if (rv != BCM_E_NONE) {
        printf("Error, in delete mult_adjacency \n");
        return rv;
    }

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  port_id;

    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get %d\n", port_id);
        return rv;
    }
    
    printf("Found trill port\n");
    rv = bcm_trill_port_delete(unit, trill_port.trill_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_delete %d\n", port_id);
        return rv;
    }
    printf("deleted trill port id=%d name=%d\n", trill_port.trill_port_id, trill_port.name);

    
    if (trill_bridge_mode) {    
        rv = bcm_l3_egress_get(unit, trill_port.egress_if, &trill_fec);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_egress_destroy\n");
            return rv;
        }
        printf("Found Trill FEC: 0x%x \n", trill_port.egress_if);

        rv = bcm_l3_egress_destroy(unit, trill_port.egress_if);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_egress_destroy\n");
            return rv;
        }
        printf("Deleted Trill FEC: 0x%x\n", trill_port.egress_if);

        bcm_trill_port_t_init(&trill_eep);
        l3_intf_to_trill_gport(trill_fec.intf, &(trill_eep.trill_port_id));

        rv = bcm_trill_port_get(unit, &trill_eep);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_trill_port_get 0x%x\n", trill_eep.trill_port_id);
            return rv;
        }
        printf("Found Trill EEP: 0x%x \n", trill_eep.trill_port_id);

        rv = bcm_trill_port_delete(unit, trill_eep.trill_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_trill_port_get 0x%x\n", trill_eep.trill_port_id);
            return rv;
        }
        printf("Deleted Trill EEP: 0x%x \n", trill_eep.trill_port_id);

        rv = bcm_l3_egress_get(unit, trill_eep.egress_if, &ll_eep);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_egress_destroy\n");
            return rv;
        }
        printf("Found LL EEP: 0x%x \n", trill_eep.egress_if);

        rv = bcm_l3_egress_destroy(unit, trill_eep.egress_if);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_egress_destroy\n");
            return rv;
        }
        printf("Deleted LL EEP: %d\n", trill_eep.egress_if);
    }else {
        rv = bcm_l3_egress_destroy(unit, trill_port.egress_if);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_egress_destroy\n");
            return rv;
        }
        printf("deleted l3 egr: %d\n", trill_port.egress_if);
    }

    return rv;
}

int
get_ll_eep(int unit, bcm_gport_t port_id, bcm_if_t *ll_eep) {
    int rv = 0;
    bcm_trill_port_t trill_port;
    bcm_trill_port_t trill_eep;
    bcm_l3_egress_t trill_fec;

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  port_id;

    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get %d\n", port_id);
        return rv;
    }    
    printf("Found trill port\n");
    
    if (trill_bridge_mode) {    
        rv = bcm_l3_egress_get(unit, trill_port.egress_if, &trill_fec);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l3_egress_destroy\n");
            return rv;
        }
        printf("Found Trill FEC: 0x%x \n", trill_port.egress_if);

        bcm_trill_port_t_init(&trill_eep);
        l3_intf_to_trill_gport(trill_fec.intf, &(trill_eep.trill_port_id));

        rv = bcm_trill_port_get(unit, &trill_eep);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_trill_port_get 0x%x\n", trill_eep.trill_port_id);
            return rv;
        }
        printf("Found Trill EEP: 0x%x \n", trill_eep.trill_port_id);
        
        *ll_eep = trill_eep.egress_if;

    }

    return rv;
}

int
add_transit_rbridge(int unit, int port, int nickname, bcm_gport_t port_id) {
    int rv = 0;
    bcm_trill_port_t trill_port;
    bcm_trill_port_t trill_eep;
    bcm_l3_egress_t trill_fec;
    create_l3_egress_s trill_fec_utility;
    bcm_if_t trill_fec_if_id;
    bcm_trill_rbridge_t trill_transit_entry;  

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  port_id;

    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get %d\n", port_id);
        return rv;
    }  

    rv = bcm_l3_egress_get(unit, trill_port.egress_if, &trill_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_destroy\n");
        return rv;
    }

    bcm_trill_port_t_init(&trill_eep);
    l3_intf_to_trill_gport(trill_fec.intf, &(trill_eep.trill_port_id));

    rv = bcm_trill_port_get(unit, &trill_eep);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get 0x%x\n", trill_eep.trill_port_id);
        return rv;
    }
       
    
    /* Add Transit Trill FEC:
        - Allocates a FEC, and point it to the allocated EEDB entry. */
    trill_fec_utility.out_tunnel_or_rif = trill_eep.egress_if;
    trill_fec_utility.out_gport = port;

    rv = l3__egress_only_fec__create(unit, &trill_fec_utility);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress_only_fec__create, \n");
        return rv;
    }
    trill_fec_if_id = trill_fec_utility.fec_id;
    printf("Created transit trill FEC: 0x%x \n", trill_fec_if_id);

    /* Add LEM entry for Adjacent-EEP. */
    sal_memset(&trill_transit_entry, 0, sizeof(trill_transit_entry));
	trill_transit_entry.name = nickname;
    trill_transit_entry.egress_if = trill_fec_if_id;
    rv = bcm_trill_rbridge_entry_add(unit, &trill_transit_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_rbridge_entry_add\n");
        return rv;
    }
    printf("Add transit trill \n");

    return rv;
}

int
remove_transit_rbridge(int unit, int nickname, bcm_gport_t port_id) {
    int rv = 0;
    bcm_trill_rbridge_t trill_transit_entry;     

    printf("Transit rbridge del 0x%x\n", port_id);
    if (port_id <= 0) {
        printf("Error, in mult_adjacency illegal port %d\n", port_id);
        return rv;
    }

    sal_memset(&trill_transit_entry, 0, sizeof(trill_transit_entry));
	trill_transit_entry.name = nickname;
    rv = bcm_trill_rbridge_entry_get(unit, &trill_transit_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_rbridge_entry_add\n");
        return rv;
    }
    
    rv = bcm_trill_rbridge_entry_delete(unit, &trill_transit_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_rbridge_entry_add\n");
        return rv;
    }
    printf("Delete transit trill \n");

    rv = bcm_l3_egress_destroy(unit, trill_transit_entry.egress_if);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_destroy, \n");
        return rv;
    }
    printf("Destroy transit trill FEC: 0x%x \n", trill_transit_entry.egress_if);

    return rv;
}

/* 
  Function that add/remove multicast adjacency entry
 
  Note: Adjacency check is currently done in CINT per Remote Rbridge
        but this is not always the case.
        Adjacency check should be done on the neighbor link (MAC address + system-port).
        In CINT we assume each neighbor link is connected to only one Remote Rbridge
        thus we take all information from Remote Rbridge.
        BUT in the real world it is not always the case as one neighbor link can be connected to many Remote Rbridges.
 
      Get:
          unit         (IN) - device number
          port_id      (IN) - trill port id
          add_adjacency(IN) - '0' - remove adjacency entry
                              '1' - add    adjacency entry
     Return:
          rv  (OUT) - error
 
*/
int
mult_adjacency(int unit, bcm_gport_t port_id, int add_adjacency) {

    bcm_trill_port_t            trill_port;
    bcm_l3_egress_t             egr;
    bcm_trill_port_t            trill_eep;
    bcm_trill_multicast_adjacency_t mult_adjacency;

    int rv = 0;

    printf(" multicast adjacency add/remove 0x%x\n", port_id);
    if (port_id <= 0) {
        printf("Error, in mult_adjacency illegal port %d\n", port_id);
        return rv;
    }

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  port_id;

    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get %d\n", port_id);
        return rv;
    }

    rv = bcm_l3_egress_get(unit, trill_port.egress_if, &egr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_get %d\n", trill_port.egress_if);
        return rv;
    }
    
    if (trill_bridge_mode) {
        bcm_trill_port_t_init(&trill_eep);
        l3_intf_to_trill_gport(egr.intf, &(trill_eep.trill_port_id));

        rv = bcm_trill_port_get(unit, &trill_eep);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_trill_port_get 0x%x\n", trill_eep.trill_port_id);
            return rv;
        }
        printf("interface object of type encap: %x\n", trill_eep.egress_if);
    } else {
        printf("interface object of type encap: %x\n", egr.encap_id);
    }

    mult_adjacency.flags=0;
    if (trill_bridge_mode) {
        mult_adjacency.encap_intf = trill_eep.egress_if;
    } else {
        mult_adjacency.encap_intf = egr.encap_id; /* LL outlif: interface object of type encap */
    }
    mult_adjacency.port = egr.port;      /* port */

    if (add_adjacency) {
        /* add adjacency entry:
         * add LEM entry: Trill Ethernet SA x port -> LL outlif
         * from LL outlif, get SA. */
        rv = bcm_trill_multicast_adjacency_add(unit, &mult_adjacency); 

        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_trill_multicast_adjacency_add, \n");
            return rv;
        }
        printf("add multicast adjacency: %d \n", port_id);
    }
    else{
        /* remove adjacency entry */
        rv = bcm_trill_multicast_adjacency_delete(unit, &mult_adjacency); 

        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_trill_multicast_adjacency_delete, \n");
            return rv;
        }
        printf("delete multicast adjacency: %d \n", port_id);
    }

    return rv;

}


/* Configure Local My Rbridge:
 * Set my nickname.
 * flags: additional flags that can be set in addition to local. 
*/ 
int
config_local_rbridge(int unit, int nickname_local, int flags) {
    int rv = 0;
    
    /* Add TRILL Virtual Port for local RB (configures my nickname) */
    bcm_trill_port_t_init(&trill_port_local);
    trill_port_local.flags = flags | BCM_TRILL_PORT_LOCAL;
    trill_port_local.name  = nickname_local;

    rv = bcm_trill_port_add(unit, &trill_port_local);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_add, \n");
        return rv;
    }
    printf("Created local trill port: %x for my-nicknmae: %x \n", trill_port_local.trill_port_id, trill_port_local.name);
    
    return rv;
}


/* Remove Local My Rbridge */ 
int
remove_local_rbridge(int unit) {
    int rv = 0;
    
    rv = bcm_trill_port_delete(unit, trill_port_local.trill_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_delete\n");
        return rv;
    }
    printf("Deleted local trill port: id=%d\n", trill_port_local.trill_port_id);
    trill_port_local.trill_port_id = -1;
    
    return rv;
}


/* create Trill port of type MC:
 * 
 * output: l3_if object of type FEC. Contains the FEC entry. 
 * output: trill_Port
 */
int
add_trill_port_mc(int unit, int mc_group, int nickname, int fec_id, int *eg_if, bcm_trill_port_t *trill_port) {
    int rv = 0;
    create_l3_egress_s egr_obj;
    bcm_if_t    egr_if_id;
    int         flags = 0;

    /* create FEC of type Trill MC: contains MC-ID + distribution tree */
        
    /* create FEC of type Trill MC: contains MC-ID */
    BCM_GPORT_MCAST_SET(egr_obj.out_gport, mc_group );
    egr_obj.l3_flags    = BCM_L3_TRILL_ONLY ;
   
    if (fec_id != -1) {
        egr_obj.fec_id = fec_id;
        egr_obj.allocation_flags = BCM_L3_WITH_ID;
    }

    rv = l3__egress__create(unit,&egr_obj);
    BCM_IF_ERROR_RETURN(rv);
    *eg_if = egr_obj.fec_id;
    printf("Created egress obj: 0x%x \n", *eg_if);

    /* update FEC of type Trill MC: add distribution tree */
    bcm_trill_port_t_init(trill_port);
    trill_port->egress_if  =  *eg_if;
    trill_port->name       =  nickname;
    trill_port->flags      = BCM_TRILL_PORT_MULTICAST;
    
    rv = bcm_trill_port_add(unit, trill_port);
    BCM_IF_ERROR_RETURN(rv);
    printf("Created trill_port: 0x%x 0x%x\n", trill_port->trill_port_id, trill_port->name );
    
    return rv;

}


/* remote Trill port of type MC:
 * 
 * output: l3_if object of type FEC. Contains the FEC entry. 
 * output: trill_Port
 */
int
remove_trill_port_mc(int unit, bcm_gport_t port_id) {
    int rv = 0;
    bcm_trill_port_t trill_port;

    printf("remove_trill_port_mc %d\n", port_id);
    if (port_id <= 0) {
        printf("Error, in remove_trill_port_mc illegal id %d\n", port_id);
        return rv;
    }

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  port_id;

    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get %d\n", port_id);
        return rv;
    }
    
    printf("Found trill port\n");
    rv = bcm_trill_port_delete(unit, trill_port.trill_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_delete %d\n", port_id);
        return rv;
    }
    printf("deleted trill port id=%d name=%d\n", trill_port.trill_port_id, trill_port.name);

    rv = bcm_l3_egress_destroy(unit, trill_port.egress_if);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_destroy\n");
        return rv;
    }
    printf("deleted egress obj: %d\n", trill_port.egress_if);
    
    return rv;

}


/* Set flooding group to an exisiting MC trill port.                                      */
/* This method cannot be used for a UC trill port because the flooding uses FEC (egr obj) */
/* which is used for the UC case.                                                         */
int 
assign_flooding_to_MC_trill_port(int unit, bcm_gport_t id, int vsi, int flood_group ) {
    int                         rv;
    bcm_vlan_control_vlan_t     vsi_control;
    bcm_trill_port_t            trill_port;
    create_l3_egress_s          egr_obj;
    int                         eg_if=0;
    int                         flags = 0;

    BCM_GPORT_MCAST_SET(egr_obj.out_gport, flood_group);
    egr_obj.l3_flags    = BCM_L3_TRILL_ONLY ;
    
    rv = l3__egress__create(unit,&egr_obj);
    BCM_IF_ERROR_RETURN(rv);
    eg_if = egr_obj.fec_id;

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  id;

    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get\n");
        return rv;
    }
    trill_port.flags  =  BCM_TRILL_PORT_MULTICAST | BCM_TRILL_PORT_REPLACE;
    trill_port.egress_if  =  eg_if;

    rv = bcm_trill_port_add(unit, &trill_port);
    BCM_IF_ERROR_RETURN(rv);

    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    vsi_control.unknown_unicast_group   = flood_group;
    vsi_control.unknown_multicast_group = flood_group;
    vsi_control.broadcast_group         = flood_group;
    
    
    rv = bcm_vlan_control_vlan_set(unit,vsi,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }

    printf ("Set flooding to vsi %d .\n", vsi);

    /*
    rv=create_mc_group(unit,flood_group,1,1);
    BCM_IF_ERROR_RETURN(rv);
    */
    return rv;
}
int 
assign_flooding_group_to_vsi(int unit, int vsi, int flood_group ) {
    int rv;
    bcm_vlan_control_vlan_t vsi_control;

    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    vsi_control.unknown_unicast_group   = flood_group;
    vsi_control.unknown_multicast_group = flood_group;
    vsi_control.broadcast_group         = flood_group;
    
    
    rv = bcm_vlan_control_vlan_set(unit,vsi,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }
    printf ("Set flooding to vsi %d .\n", vsi);

   return rv;

}

/* Creating virtual MC group
   Input: FEC/ECMP
   Output Virtual MC-ID group
       Virtual MC-ID group = subport type + FEC/ECMP */
int
create_virtual_mc_group(int unit, int ecmp, int *vir_mc_id) {
    int rv;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_SUBPORT | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ecmp);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Created virtual MC group = %d .\n", ecmp);

    *vir_mc_id = ecmp;

    return rv;
}

int
create_mc_group(int unit, int group, int with_trill_copies, int with_host_copies) {
    int rv;
    bcm_if_t encap_id;
    bcm_trill_multicast_entry_t trill_mc_entry;
    bcm_if_t                    host_encap;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Created MC group = 0x%x .\n", group);

 
    if (with_trill_copies) {
        if (!trill_bridge_mode) {
            rv = bcm_multicast_trill_encap_get(unit, -1, trill_port_rb1.trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
        } else {
            rv = bcm_multicast_trill_encap_get(unit, -1, dist_tree_1_port_nb[0].trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
            encap_id = encap_id & 0x3ffff;
        }
        rv = bcm_multicast_egress_add(unit, group, port_1, encap_id);
        BCM_IF_ERROR_RETURN(rv);
        printf ("Added port %d with encap_id 0x%x to group 0x%x. (Trill copy)\n", port_1, encap_id, group);

                                                                                                    
        if (!trill_bridge_mode) {
            rv = bcm_multicast_trill_encap_get(unit, -1, trill_port_rb2.trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
        } else {
            rv = bcm_multicast_trill_encap_get(unit, -1, dist_tree_1_port_nb[1].trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
            encap_id = encap_id & 0x3ffff;
        }                     
        rv = bcm_multicast_egress_add(unit, group, port_2, encap_id);                             
        BCM_IF_ERROR_RETURN(rv);                                                                     
        printf ("Added port %d with encap_id 0x%x to group 0x%x. (Trill copy)\n", port_2, encap_id, group);
    }

    if (with_host_copies) {
       host_encap = get_encap_id_to_bridge(unit);

       rv = bcm_multicast_egress_add(unit,  group, port_1, host_encap);
       BCM_IF_ERROR_RETURN(rv);
       printf ("Added port %d to group 0x%x. (ETH copy)\n", port_1, group);
       
       rv = bcm_multicast_egress_add(unit, group, port_2, host_encap);
       BCM_IF_ERROR_RETURN(rv);
       printf ("Added port %d to group 0x%x. (ETH copy)\n", port_2, group);
    }

   return rv;
}
int
remove_mc_group(int unit, int group, int with_trill_copies, int with_host_copies) {
    int rv;
    bcm_if_t encap_id;
    bcm_if_t host_encap;

    if (with_trill_copies) {
        if (!trill_bridge_mode) {
            rv = bcm_multicast_trill_encap_get(unit, -1, trill_port_rb1.trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
        } else {
            rv = bcm_multicast_trill_encap_get(unit, -1, dist_tree_1_port_nb[0].trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
            encap_id = encap_id & 0x3ffff;
        }
        printf ("TRILL encap = 0x%x \n", encap_id);

        /* Delete port + encapsulation from MC group */
        rv = bcm_multicast_egress_delete(unit, group, port_1, encap_id);
        BCM_IF_ERROR_RETURN(rv);
        
         /* Get trill encapsulation */   
        if (!trill_bridge_mode) {
            rv = bcm_multicast_trill_encap_get(unit, -1, trill_port_rb2.trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
        } else {
            rv = bcm_multicast_trill_encap_get(unit, -1, dist_tree_1_port_nb[1].trill_port_id ,0, &encap_id); 
            BCM_IF_ERROR_RETURN(rv);
            encap_id = encap_id & 0x3ffff;
        }
        printf ("TRILL encap = 0x%x \n", encap_id);

        /* Delete port + encapsulation from MC group */
        rv = bcm_multicast_egress_delete(unit, group, port_2, encap_id);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (with_host_copies) {
        host_encap = get_encap_id_to_bridge(unit);

        rv = bcm_multicast_egress_delete(unit, group, port_1, host_encap);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_multicast_egress_delete(unit, group, port_2, host_encap);
        BCM_IF_ERROR_RETURN(rv);
    }
    
    rv = bcm_multicast_destroy(unit,group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Destroy MC group = 0x%x \n", group);

   return rv;
}
int
create_transit_mc_group(int unit, int group, int with_trill_copies, int with_host_copies) {
    int rv;
    bcm_if_t encap_id;
    bcm_if_t host_encap;

    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Created Transit MC group = 0x%x .\n", group);

    if (with_trill_copies) {
        rv = get_ll_eep(unit, trill_port_rb1.trill_port_id, &encap_id);
        BCM_IF_ERROR_RETURN(rv); 
        encap_id = encap_id & 0x3ffff; 
        rv = bcm_multicast_egress_add(unit, group, port_1, encap_id);
        BCM_IF_ERROR_RETURN(rv);
        printf ("Added port %d to group 0x%x. (Trill copy)\n", port_1, group);
        
        rv = get_ll_eep(unit, trill_port_rb2.trill_port_id, &encap_id);
        BCM_IF_ERROR_RETURN(rv); 
        encap_id = encap_id & 0x3ffff;                                                                                                      
        rv = bcm_multicast_egress_add(unit, group, port_2, encap_id);                             
        BCM_IF_ERROR_RETURN(rv);                                                                     
        printf ("Added port %d to group 0x%x. (Trill copy)\n", port_2, group);                       
    }

    if (with_host_copies) {
       host_encap = get_encap_id_to_bridge(unit);

       rv = bcm_multicast_egress_add(unit,  group, port_1, host_encap);
       BCM_IF_ERROR_RETURN(rv);
       printf ("Added port %d to group 0x%x. (ETH copy)\n", port_1, group);
       
       rv = bcm_multicast_egress_add(unit, group, port_2, host_encap);
       BCM_IF_ERROR_RETURN(rv);
       printf ("Added port %d to group 0x%x. (ETH copy)\n", port_2, group);
    }

   return rv;
}

int
remove_transit_mc_group(int unit, int group, int with_trill_copies, int with_host_copies) {
    int rv;
    bcm_if_t encap_id;
    bcm_if_t host_encap;
    
    if (with_trill_copies) {
        /* Delete port + encapsulation from MC group */
        rv = get_ll_eep(unit, trill_port_rb1.trill_port_id, &encap_id);
        BCM_IF_ERROR_RETURN(rv); 
        encap_id = encap_id & 0x3ffff; 
        printf ("TRILL encap = %d \n", encap_id);
        rv = bcm_multicast_egress_delete(unit, group, port_1, encap_id);
        BCM_IF_ERROR_RETURN(rv);
        

        /* Delete port + encapsulation from MC group */
        rv = get_ll_eep(unit, trill_port_rb2.trill_port_id, &encap_id);
        BCM_IF_ERROR_RETURN(rv); 
        encap_id = encap_id & 0x3ffff;  
        printf ("TRILL encap = %d \n", encap_id);
        rv = bcm_multicast_egress_delete(unit, group, port_2, encap_id);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (with_host_copies) {
        host_encap = get_encap_id_to_bridge(unit);

        rv = bcm_multicast_egress_delete(unit, group, port_1, host_encap);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_multicast_egress_delete(unit, group, port_2, host_encap);
        BCM_IF_ERROR_RETURN(rv);
    }

   return rv;
}

int
create_flooding_port(int unit, int vsi, int mc_group, int nick, bcm_trill_port_t* trill_port) {
    int rv;
    bcm_if_t trill_encap;
    bcm_trill_multicast_entry_t trill_mc_entry;
    bcm_if_t                    host_encap;
    int                         egr_if;
    int vir_mc_id;

    rv = create_mc_group(unit, mc_group, 1, 1);  
    BCM_IF_ERROR_RETURN(rv);
    
    rv = add_trill_port_mc(unit, mc_group, nick, -1 /* flood_fec*/, &egr_if, trill_port); 
    BCM_IF_ERROR_RETURN(rv);
    printf ("Created flooding trill port  id=%d nick=%d \n", trill_port->trill_port_id, trill_port->name);

    if(!mc_id){
	   /* Creating virtual MC group that actually has FEC/ECMP index
        will be used for setting VSI->FEC/ECMP */
        rv = create_virtual_mc_group(unit, egr_if, &vir_mc_id);
	}

    /* old implementation that configure sw db (mc-id to nickname)*/
    if (mc_id) {

        bcm_trill_multicast_entry_t_init(&trill_mc_entry); 
        trill_mc_entry.root_name = nick;
        trill_mc_entry.c_vlan = vsi;
        trill_mc_entry.group = mc_group;

        rv = bcm_trill_multicast_entry_add(unit, &trill_mc_entry);
        BCM_IF_ERROR_RETURN(rv);
        printf ("Created trill_multicast_entry\n");
    }
    bcm_vlan_control_vlan_t vsi_control;

    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    if(mc_id)
	{
	    vsi_control.unknown_unicast_group   = mc_group;
        vsi_control.unknown_multicast_group = mc_group;
        vsi_control.broadcast_group         = mc_group;
    }else
	{
	    vsi_control.unknown_unicast_group   = vir_mc_id;
        vsi_control.unknown_multicast_group = vir_mc_id;
        vsi_control.broadcast_group         = vir_mc_id;
	}
    
    
    rv = bcm_vlan_control_vlan_set(unit,vsi,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }
    printf ("Set flooding to vsi %d .\n", vsi);

   return rv;
}
int
remove_flooding_port(int unit, int vsi, int mc_group, int nick, int flood_port_id ) {
    int rv = 0;
    bcm_if_t                    trill_encap;
    bcm_trill_multicast_entry_t trill_mc_entry;
    bcm_if_t                    host_encap;
    bcm_gport_t     p[10];   
    bcm_if_t        e[10];
    int             port_cnt;
    int             i;

    printf ("Remove flooding port: MC=%d nick=%d vsi=%d\n", mc_group, nick, vsi);

    /* old implementation that configure sw db (mc-id to nickname)*/
    if (mc_id){

        bcm_trill_multicast_entry_t_init(&trill_mc_entry);
        trill_mc_entry.root_name = nick;
        trill_mc_entry.c_vlan = vsi;
        trill_mc_entry.group = mc_group;
      
        rv = bcm_trill_multicast_entry_delete(unit, &trill_mc_entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = remove_trill_port_mc(unit,flood_port_id);
    BCM_IF_ERROR_RETURN(rv);


    rv = bcm_multicast_egress_get(unit,mc_group,10,p,e,&port_cnt);
    BCM_IF_ERROR_RETURN(rv);

    for (i=0; i< port_cnt; i++) {
        rv = bcm_multicast_egress_delete(unit, mc_group, p[i], e[i]);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = bcm_multicast_destroy(unit, mc_group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("deleted MC flooding group  = %d \n", mc_group);
    bcm_vlan_control_vlan_t vsi_control;

    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    vsi_control.unknown_unicast_group   = 0;
    vsi_control.unknown_multicast_group = 0;
    vsi_control.broadcast_group         = 0;
    
    
    rv = bcm_vlan_control_vlan_set(unit,vsi,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }
    printf ("Set flooding to vsi %d .\n", vsi);


    return rv;
}
int traps_set(int unit) {
  int rv = BCM_E_NONE;
  bcm_rx_trap_config_t config;
  bcm_rx_trap_core_config_t arr_core[2];
  int flags = 0;

  if (is_traps_set) {
      return rv;
  }
  bcm_rx_trap_config_t_init(&config);
  bcm_rx_trap_t traps[] = {bcmRxTrapTrillCite,
                           bcmRxTrapTrillChbh,
                           bcmRxTrapTrillDesignatedVlanNoMymac,
                           bcmRxTrapIgmpMembershipQuery,
                           bcmRxTrapFrwrdIgmpMembershipQuery,
                           bcmRxTrapEgTrillHairPinFilter,
                           bcmRxTrapEgTrillBounceBack,                    
                           bcmRxTrapEgTrillTtl0};
  int trap_i;

  config.flags = (BCM_RX_TRAP_UPDATE_DEST); 
  config.trap_strength = 7;
  config.dest_port=BCM_GPORT_BLACK_HOLE;

  if (is_device_or_above(unit, JERICHO)) {
    arr_core[0].dest_port = BCM_GPORT_BLACK_HOLE;
    arr_core[1].dest_port = BCM_GPORT_BLACK_HOLE;
    config.core_config_arr = arr_core;
    config.core_config_arr_len = 2;
  }

  for (trap_i = 0; trap_i < sizeof(traps) / sizeof(traps[0]); ++trap_i) {
    trap_id[trap_i]=0xff;

    rv = bcm_rx_trap_type_create(unit,flags,traps[trap_i],&trap_id[trap_i]);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap %d \n", trap_i);
        return rv;
    }
    
    rv = bcm_rx_trap_set(unit,trap_id[trap_i],&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }
    trap_id_cnt++;
  }

  rv = bcm_rx_trap_type_create(unit,0x0,bcmRxTrapAdjacentCheckFail,&adj_trap);
  if (rv != BCM_E_NONE) {
      printf("Error, in trap create, trap %d \n", trap_i);
      return rv;
  }

  config.trap_strength = 0x0;
  rv = bcm_rx_trap_set(unit,adj_trap,&config);
  if (rv != BCM_E_NONE) {
      printf("Error, in trap set \n");
      return rv;
  }
  is_traps_set = TRUE;
  return rv;
}
int traps_unset(int unit) {
  int rv = BCM_E_NONE;
  int trap_i;

  if (!is_traps_set) {
      return rv;
  }


  for (trap_i = 0; trap_i < trap_id_cnt; trap_i++) {
    rv = bcm_rx_trap_type_destroy(unit,trap_id[trap_i]);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }
    trap_id[trap_i]=0xff;
    
  }
  trap_id_cnt = 0;

  if (is_same_interface_filter_enabled) {
      rv = bcm_rx_trap_type_destroy(unit,trap_id_same_interface);
      BCM_IF_ERROR_RETURN(rv);
      is_same_interface_filter_enabled = 0;
  }

  rv = trill_in_lif_field_group_unset(unit, 5, 6);  
  if (rv != BCM_E_NONE) {
      printf("Error, in trill_in_lif_field_group_unset, trap id $adj_trap \n");
      return rv;
  }
  
  rv = bcm_rx_trap_type_destroy(unit,adj_trap);
  if (rv != BCM_E_NONE) {
      printf("Error, in trap create, trap id $adj_trap \n");
      return rv;
  }
  adj_trap=0xff;
  is_traps_set = FALSE;
  return rv;
}
int
add_root_rbridge(int unit, int rootname, bcm_trill_port_t *trill_port, bcm_multicast_t flooding_mc) {
    int rv = 0;
    bcm_l3_egress_t egr_obj_rb;
    bcm_if_t egr_if_id_rb;
    create_l3_egress_s egr_obj;
    int         flags = 0;
    int         eg_if=0;

    /* create MC FEC for flooding with MC group */
    if (flooding_mc != -1) {
        BCM_GPORT_MCAST_SET(egr_obj.out_gport, flooding_mc );
        egr_obj.l3_flags    = BCM_L3_TRILL_ONLY ;
   
        rv = l3__egress__create(unit,&egr_obj);
        BCM_IF_ERROR_RETURN(rv);
        eg_if = egr_obj.fec_id;
        printf("Created egress obj: %d \n", eg_if);

    }

    /* update MC FEC for flooding with dist-tree.
     * Save in sw: trill_port_id -> dist tree, gport type: TRILL-MC
                   trill_port_id -> fec_id: -1, type: TRILL-MC */
    bcm_trill_port_t_init(trill_port);
    trill_port->flags  =  BCM_TRILL_PORT_MULTICAST;
    trill_port->name       =  rootname;
    trill_port->egress_if  =  eg_if;

    rv = bcm_trill_port_add(unit, trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_add, \n");
        return rv;
    }
    printf ("Created root bridge trill port: %d for %d \n", trill_port->trill_port_id, rootname );

    return rv;
}
int
add_root_rbridge_nb(int unit, int rootname, bcm_if_t ll_eep_id, bcm_trill_port_t *trill_port, bcm_multicast_t flooding_mc) {
    int rv = 0;

    /* update MC FEC for flooding with dist-tree.
     * Save in sw: trill_port_id -> dist tree, gport type: TRILL-MC
                   trill_port_id -> fec_id: -1, type: TRILL-MC */
    bcm_trill_port_t_init(trill_port);
    trill_port->flags  =  BCM_TRILL_PORT_EGRESS | BCM_TRILL_PORT_MULTICAST;
    trill_port->name       =  rootname;
    trill_port->virtual_name = nickname_rb_local;
    trill_port->egress_if  =  ll_eep_id;

    rv = bcm_trill_port_add(unit, trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_add, \n");
        return rv;
    }
    printf ("Created root bridge trill port: %d for %d \n", trill_port->trill_port_id, rootname );

    return rv;
}
int
remove_root_rbridge(int unit, bcm_gport_t port_id) {
    bcm_trill_port_t            trill_port;
    int rv = 0;

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id = port_id;

    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_get\n");
        return rv;
    }

    printf("Found trill port\n");
    rv = bcm_trill_port_delete(unit, trill_port.trill_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_delete\n");
        return rv;
    }
    printf("deleted trill port id=%d name=%d\n", trill_port.trill_port_id, trill_port.name);

    return rv;
}
int
mc_global(int unit) {
    int rv = 0;

    /* Configure TRILL_ALL_RBRIDGES Upper 3 bytes of MAC address to match */ 
    rv = bcm_switch_control_set(unit, bcmSwitchTrillBroadcastDestMacOui, 0x0180C2);
    BCM_IF_ERROR_RETURN(rv);

    /* Configure TRILL_ALL_RBRIDGES Lower 3 bytes of MAC address to match */
    rv = bcm_switch_control_set(unit, bcmSwitchTrillBroadcastDestMacNonOui, 0x999990);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}
int
mc_global_reset(int unit) {
    int rv = 0;

    /* Configure TRILL_ALL_RBRIDGES Upper 3 bytes of MAC address to match */ 
    rv = bcm_switch_control_set(unit, bcmSwitchTrillBroadcastDestMacOui, 0x0);
    BCM_IF_ERROR_RETURN(rv);

    /* Configure TRILL_ALL_RBRIDGES Lower 3 bytes of MAC address to match */
    rv = bcm_switch_control_set(unit, bcmSwitchTrillBroadcastDestMacNonOui, 0x0);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}
/*
 * configure port to enable recycling.
 * For transit trill multi-destination:
 * 1st pass send trill copies, 2nd pass, send local copies.
 */
int port_config_recycle_port(int unit, 
                             bcm_port_t *recycle_port, 
                             int recycle_port_num,
                             bcm_port_t *port, 
                             int port_num,
                             bcm_vlan_t vid_desig, 
                             bcm_gport_t *snoop_trap_gport_id,
                             bcm_field_group_t *snoop_group,
                             int is_ptch) {
    int rv = 0;
    int i;
    uint32 flag;
    bcm_port_interface_info_t intf;
    bcm_port_mapping_info_t map;


    for (i = 0; i < recycle_port_num; i++) {
        rv = port_config(unit, recycle_port[i], vid_desig);
        BCM_IF_ERROR_RETURN(rv);

        /* 
          We want this to snoop packets to the recycle port (dest_local_port).
          Once the packet arrives on the 2nd pass, we do not want to snoop it again.
          We will change the TM-Port.Key-Gen-Var-Profile for the recycle port,
          and add two rules:
          higher priority rule for the recycle port, that will not do anything
          2nd rule that will snoop, for all traffic except recycle port
          */
        rv = bcm_port_class_set(unit, recycle_port[i], bcmPortClassFieldIngress, recycle_port_num);
        BCM_IF_ERROR_RETURN(rv);

        /* snoop_trap_gport_id defined in cint_trill.c */
        rv = two_pass_snoop_set(unit, bcmRxTrapUserDefine, recycle_port[i], &snoop_trap_gport_id[i]);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = trill_2pass_field_group_set(unit, snoop_trap_gport_id, recycle_port_num, snoop_group);
    BCM_IF_ERROR_RETURN(rv);

    for (i = 0; i < port_num; i++) {
        rv = bcm_port_get(unit, port[i], &flag, &intf, &map);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_port_class_set(unit, port[i], bcmPortClassFieldIngress, map.core);
        BCM_IF_ERROR_RETURN(rv);

        if (is_ptch) {
            rv = system_port_lookup_ptch(unit, 20, 17, port[i], recycle_port[map.core]); 
            BCM_IF_ERROR_RETURN(rv);
        }   
    }

    for (i = 0; i < recycle_port_num; i++) {
        rv = bcm_port_control_set(unit, recycle_port[i], bcmPortControlOverlayRecycle, 1);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}
int
trill_uni(int unit) {
    int rv = BCM_E_NONE;
    bcm_vlan_t
        vsi_1_bridge,
        vsi_2_desig;
 
    bcm_l2_addr_t     l2_addr; 

    /* For Jericho, we use the FGL to configure the VL */
    if (is_device_or_above(unit, JERICHO)) {
        rv = trill_uni_fgl(unit, 0);
        return rv;
    }

    if (trill_uni_configured) {
        return rv;
    }
    printf("trill_uni %d %d\n",port_1, port_2);

    /* Initialize trill module (SW related databases) */
    rv = bcm_trill_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_init\n");
        return rv;
    }
    
    /* Global config:
       - update inLif to Trill inLif using PMF  
       - Hop count
       - EtherType
       - Acceptable frame type
       - Port -> Vlan domain
       - Designated Vlan 
        */
    rv = global_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }
    
    rv = port_config(unit, port_1, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_1\n");
        return rv;
    }
    
    rv = port_config(unit, port_2, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_2\n");
        return rv;
    }
    
    /* Create vlan:
       - open VSI (VSI == VID)
       - Add ports
       - Map <VD,vid> -> lif -> VSI */    
    
    rv = vlan_init(unit, vid_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    vsi_1_bridge = vid_1;
    
    rv = vlan_port_init(unit, vid_1, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }
   
    rv = vlan_init(unit, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }
    vsi_2_desig = vid_2_desig;        
    
    rv = vlan_port_init(unit, vid_2_desig, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vid_1, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vid_2_desig, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }  
    
    /* Add a <VD,vid> -> lif -> VSI mapping */
    /* No need to add port_2, since it's on the same vlan domain as port_1 */
    rv = vlan_port_set(unit, port_1, vid_2_desig, vsi_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_set: vid=%d, vsi=%d\n",vid_2_desig,vsi_2_desig);
        return rv;
    }
    
     /* Add a <VD,vid> -> lif -> VSI mapping */
    /* No need to add port_2, since it's on the same vlan domain as port_1 */
    rv = vlan_port_set(unit, port_1, vid_1, vsi_1_bridge);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_set: vid=%d, vsi=%d\n",vid_1,vsi_1_bridge);
        return rv;
    }
    
    
                    
    /* Create L3 interfaces for TRILL: Configures MyMac  */
    rv = l3_intf_create(unit, trill_vsi_mac_2, vsi_2_desig, &l3_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3_intf_create: vsi=%d\n",vsi_2_desig );
        return rv;
    }
    
    /* Add TRILL Virtual Port for local RB (configures my nickname) */
    rv = config_local_rbridge(unit, nickname_rb_local, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in config_local_rbridge: vsi=$vsi_1_bridge\n");
        return rv;
    }

    /* Add remote RBridge:
       - Allocates an EEDB entry, and configures MAC + Out-VSI
       - Allocates a FEC, and point it to the allocated EEDB entry.
      - Binds (SW) nickname and egress_if (FEC)
      - Adds adjacent mac -> eep mapping (taken from egr_if) */
    rv = add_remote_rbridge(unit, remote_mac_rb1, vsi_2_desig, port_1, nickname_rb1, 0, &trill_port_rb1);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=$vsi_2_desig\n");
        return rv;
    }
    
    rv = add_remote_rbridge(unit, remote_mac_rb2, vsi_2_desig, port_2, nickname_rb2, 0, &trill_port_rb2);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi_2_desig );
        return rv;
    }
       
    /* Add L2 entry for Customer packet to TRILL tunnel:
       Adds an entry to the LEM with <c_dmac, nickname (taken from trill_port)> -> FEC */
    bcm_l2_addr_t_init(&l2_addr, c_dmac, vid_1);
    l2_addr.port = trill_port_rb1.trill_port_id;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

/* Add L2 entry for Customer packet to TRILL tunnel: for egress rb flow
       Adds an entry to the LEM with <da= host1_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host1_mac, vid_1);
    l2_addr.port = port_1;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }


/* Add L2 entry for Customer packet to TRILL tunnel: for bridge flow
       Adds an entry to the LEM with <da= host4_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host4_mac, vid_1);
    l2_addr.port = port_2;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }


    trill_uni_configured = TRUE;

    return rv;
}

int
trill_mc_transit_fecless(int unit) {
    return trill_mc_transit_fecless_config(unit,0, 0, 0);
}
int
trill_mc_transit_fecless_config(int unit, int is_fgl, int double_tag, int is_2pass) {
    int rv = 0;
    bcm_if_t encap_id;
    uint32 flags;
    /*bcm_trill_port_t root_port;*/
    bcm_l2_addr_t     l2_addr; 
    bcm_trill_multicast_entry_t mc_entry;
    bcm_if_t              host_encap;
    bcm_field_group_t rpf_group = 100;
    int qual_id_ing_nickname;
    int qual_id_eg_nickname;
    bcm_if_t ll_eep_id;


    /*enable RPF Trap*/
    rv = rpf_drop_set(unit,10, rpf_group, &qual_id_ing_nickname, &qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);

    if (is_fgl) {
        rv = trill_uni_fgl(unit, double_tag);
    } else {
        rv = trill_uni(unit);
    }
    BCM_IF_ERROR_RETURN(rv);
    
    rv = mc_global(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (trill_bridge_mode) {
        rv = get_ll_eep(unit, trill_port_rb1.trill_port_id, &ll_eep_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = add_root_rbridge_nb(unit, dist_tree_1, ll_eep_id, &dist_tree_1_port_nb[0], -1);
        BCM_IF_ERROR_RETURN(rv);
        
        rv = get_ll_eep(unit, trill_port_rb2.trill_port_id, &ll_eep_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = add_root_rbridge_nb(unit, dist_tree_1, ll_eep_id, &dist_tree_1_port_nb[1], -1);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = add_root_rbridge(unit, dist_tree_1, &dist_tree_1_port, -1);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Create Multicast Group */  
    rv = create_mc_group(unit, l2mc_group, 1, !is_2pass);
    BCM_IF_ERROR_RETURN(rv);

    if (trill_bridge_mode) {
        /* Create Multicast Group */  
        rv = create_transit_mc_group(unit, trill_mc_group, 1, !is_2pass);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* bind MC group to distribution-tree */
    bcm_trill_multicast_entry_t_init(&mc_entry);
    mc_entry.root_name = dist_tree_1;
    if (is_fgl) {
        if (double_tag) {
            mc_entry.c_vlan = FGL_outer_vid;
            mc_entry.c_vlan_inner = FGL_inner_vid;
        } else {
            mc_entry.c_vlan = vid_1;
            mc_entry.c_vlan_inner = BCM_VLAN_INVALID; 
        }
    } else {
        mc_entry.c_vlan = vid_1;
    }

    mc_entry.group = (trill_bridge_mode ? trill_mc_group : l2mc_group);
    rv =  bcm_trill_multicast_entry_add(unit, &mc_entry);
    BCM_IF_ERROR_RETURN(rv);

    bcm_l2_addr_t_init(&l2_addr, c_dmac, vid_1);
    l2_addr.l2mc_group  = l2mc_group;
    if (!trill_bridge_mode) {
        l2_addr.port = dist_tree_1_port.trill_port_id;
    }

    /* Add mac addr with forward to MC group*/
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    /* Add RPF-Check entry to the TCAM */
    if (!trill_bridge_mode) {
        rv = bcm_multicast_trill_encap_get(unit, l2mc_group, trill_port_rb1.trill_port_id ,0, &encap_id); 
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = get_ll_eep(unit, trill_port_rb1.trill_port_id, &encap_id);
        BCM_IF_ERROR_RETURN(rv);
        encap_id = encap_id & 0x3ffff; 
    }
    printf ("TRILL encap for RPF check (Adj-eep)= %d \n", encap_id);
    
    rv = trill_multicast_source_add(unit, dist_tree_1, nickname_rb1, port_1,encap_id, rpf_group,
                                         qual_id_ing_nickname, qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);

    
    /* this is in order to check the trap - mc key not found  (add this to vtt lookup at the TCAM)*/
    if (trill_bridge_mode) {
        rv = get_ll_eep(unit, trill_port_rb1.trill_port_id, &ll_eep_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = add_root_rbridge_nb(unit, dist_tree_2, ll_eep_id, &dist_tree_2_port_nb[0], -1);
        BCM_IF_ERROR_RETURN(rv);
        
        rv = get_ll_eep(unit, trill_port_rb2.trill_port_id, &ll_eep_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = add_root_rbridge_nb(unit, dist_tree_2, ll_eep_id, &dist_tree_2_port_nb[1], -1);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = add_root_rbridge(unit, dist_tree_2, &dist_tree_2_port, -1);
        BCM_IF_ERROR_RETURN(rv);
    }
    
    rv = trill_multicast_source_add(unit, dist_tree_2, nickname_rb1, port_1,encap_id, rpf_group,
                                         qual_id_ing_nickname, qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);
    
    /* this is in order to check the trap - RPF_CHECK not found (add this to flp lookup at the LEM)*/ 
    if (trill_bridge_mode) {
        rv = get_ll_eep(unit, trill_port_rb1.trill_port_id, &ll_eep_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = add_root_rbridge_nb(unit, dist_tree_3, ll_eep_id, &dist_tree_3_port_nb[0], -1);
        BCM_IF_ERROR_RETURN(rv);
        
        rv = get_ll_eep(unit, trill_port_rb2.trill_port_id, &ll_eep_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = add_root_rbridge_nb(unit, dist_tree_3, ll_eep_id, &dist_tree_3_port_nb[1], -1);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = add_root_rbridge(unit, dist_tree_3, &dist_tree_3_port, -1);
        BCM_IF_ERROR_RETURN(rv);
    }
    
    /* Create Multicast Group */  
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &dummy_mc_group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("TRILL l2mc_group = %d (dummy_mc_group)\n", dummy_mc_group);     

    bcm_trill_multicast_entry_t_init(&mc_entry);
    mc_entry.root_name = dist_tree_3;
    mc_entry.c_vlan = vid_1;
    mc_entry.group = dummy_mc_group;
    rv = bcm_trill_multicast_entry_add(unit, &mc_entry);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}
int
trill_mc_ingress_ecmp(int unit) {
    uint32              rv = 0;
    uint32              fecs[ecmp_mc_ing_nof_ifs] = {0x1300, 0x1301, 0x1302};
    uint32              nics[ecmp_mc_ing_nof_ifs] = {0x510, 0x511,0x512};
    int                 ports[ecmp_mc_ing_nof_ifs] = {port_2,port_3,port_4};
    bcm_if_t            eg_ifs[ecmp_mc_ing_nof_ifs];
    int                 i;
    bcm_l2_addr_t       l2_addr; 
    bcm_if_t            trill_encap;

    rv = trill_uni(unit);
    BCM_IF_ERROR_RETURN(rv);
    rv = bcm_switch_control_set(unit,bcmSwitchECMPHashConfig,BCM_HASH_CONFIG_ROUND_ROBIN);
    BCM_IF_ERROR_RETURN(rv);


    /* Per MC group and Dist-tree, create trill port (tree-name, port=BCM_GPORT_MCAST_SET(mc), eg_if=fec, flags=WITH_ID&TRILL_PORT_MULTICAST) */
    for (i = 0; i < ecmp_mc_ing_nof_ifs; ++i) {
        rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ecmp_mc_ing_mc_groups[i]);
        BCM_IF_ERROR_RETURN(rv);
        printf ("Created egress MC group  = 0x%x \n", ecmp_mc_ing_mc_groups[i]);

        rv = add_trill_port_mc(unit, ecmp_mc_ing_mc_groups[i], nics[i],  fecs[i], &eg_ifs[i], &ecmp_mc_trill_ports[i]); 
        if (rv != BCM_E_NONE) {
            printf ("error in add_trill_port_mc \n");
        }
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_multicast_trill_encap_get(unit, -1, trill_port_rb1.trill_port_id ,0, &trill_encap); 
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_multicast_egress_add(unit,ecmp_mc_ing_mc_groups[i], ports[i], trill_encap);
        BCM_IF_ERROR_RETURN(rv);
        printf ("Added port %d to group 0x%x with encap 0x%x.\n", ports[i],ecmp_mc_ing_mc_groups[i], trill_encap);


    }
    
    if (cint_trill_cfg.ecmp_api_is_ecmp) {
      bcm_l3_egress_ecmp_t_init(&ecmp);
      ecmp.max_paths = ecmp_mc_ing_nof_ifs;

      /* Create ECMP: bcm_l3_egress_ecmp_create(list of eg_ifs) */
      rv = bcm_l3_egress_ecmp_create(unit, &ecmp, ecmp_mc_ing_nof_ifs, eg_ifs);
      BCM_IF_ERROR_RETURN(rv);
      printf("Created multipath interface %d\n", ecmp.ecmp_intf);
    } else {
      /* Create ECMP: bcm_l3_egress_ecmp_create(list of eg_ifs) */
      rv = bcm_l3_egress_multipath_max_create(unit, 0, ecmp_mc_ing_nof_ifs, ecmp_mc_ing_nof_ifs, eg_ifs, &ecmp_mpintf);
      BCM_IF_ERROR_RETURN(rv);
      printf("Created multipath interface %d\n", ecmp_mpintf);
    }

    /* old implementation creates ecmp port */
    if (create_ecmp_port) {

        /* Create ecmp trill port (flags=BCM_TRILL_MULTIPATH, eg_if=ecmp_eg_if) */
        bcm_trill_port_t_init(&ecmp_trill_port);
     
        ecmp_trill_port.name = ecmp_tree;
        if (cint_trill_cfg.ecmp_api_is_ecmp) {
          ecmp_trill_port.egress_if = ecmp.ecmp_intf;
        } else {
          ecmp_trill_port.egress_if = ecmp_mpintf;
        }
        /*ecmp_trill_port.flags = BCM_TRILL_MULTIPATH;*/
        
        rv = bcm_trill_port_add(unit, &ecmp_trill_port);
        BCM_IF_ERROR_RETURN(rv);
        printf("Created ecmp trill port \n" );

       /* Add ecmp trill port to MACT using bcm_l2_addr_add. */
       bcm_l2_addr_t_init(&l2_addr, c_dmac, vid_1);
       l2_addr.port  = ecmp_trill_port.trill_port_id;
    } else { /* new implementation using FWD_GROUP*/
       /* Initialize l2 addr. */
        bcm_l2_addr_t_init(&l2_addr, c_dmac, vid_1);
        if (cint_trill_cfg.ecmp_api_is_ecmp) {
            BCM_GPORT_FORWARD_PORT_SET(l2_addr.port, ecmp.ecmp_intf);
        } else {
            BCM_GPORT_FORWARD_PORT_SET(l2_addr.port, ecmp_mpintf);
        }
    }
    /* Add mac addr with forward to ECMP */
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);
    
    return rv;
}
int
trill_uni_run_with_defaults(int unit){ 
int rv;

    rv = trill_uni(unit, port_1, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in trill_uni\n");
        return rv;
    }

    rv = create_flooding_port(unit, vid_1, flood_mc_group, flood_nick, &flood_trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_flooding_port\n");
        return rv;
    }

}

int rpf_drop_set(/* in */ int unit,
                 /* in */ int group_priority,
                 /* in */ bcm_field_group_t group,
                 /* out */ int *qual_id_ing_nickname,
                 /* out */ int *qual_id_eg_nickname) {
  int result;
  int auxRes;
  int entry_cnt = 2;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_group_t grp;
  bcm_field_entry_t ent;
  bcm_field_data_qualifier_t dq_trill[3]; /* 0 - Trill M-Bit, 1 - Ingress NickName, 2 - Egress NickName */
  uint16 dq_offset[3] = {0, 4, 2};
  uint8 dq_length[3] = {1, 2, 2};
  bcm_field_data_offset_base_t offset_base[3] = {bcmFieldDataOffsetBaseFirstHeader, 
      bcmFieldDataOffsetBaseFirstHeader, bcmFieldDataOffsetBaseFirstHeader};
  uint32 dq_ndx, nof_dqs = 3;
  uint8 mbit_data, mbit_mask;
  int class_port;
  uint8 data, mask;
  int is_single_core = 0;

  is_single_core = (SOC_DPP_DEFS_GET_NOF_CORES(unit) > 1) ? FALSE : TRUE;

  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&dq_trill[dq_ndx]); 
      dq_trill[dq_ndx].offset_base = offset_base[dq_ndx]; 
      /* Inner Ethernet header after Trill and Eth */
      /* trill HEADER:
         V (2b) , R (2b), M (1b) ...
         We want the M-Bit, which is the 3rd byte. We count bits from right to left, and mbit is the 
         4th bit, so the data and mask will be 0x8. */
      /* For Ingress Nickname, jump 4 Bytes and take 2 Bytes */
      /* For Egress Nickname, jump 2 Bytes and take 2 Bytes */
      dq_trill[dq_ndx].offset = dq_offset[dq_ndx]; 
      dq_trill[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &dq_trill[dq_ndx]);
      if (BCM_E_NONE != result) {
        print bcm_field_show(unit,"");
        return result;
      }      
  }
  *qual_id_ing_nickname = dq_trill[1].qual_id;
  *qual_id_eg_nickname = dq_trill[2].qual_id;

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyHeaderFormat);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassPort);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcHit); /* LEM 1st lookup found of EEP */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcValue); /* LEM 1st lookup result = EEP */
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      result = bcm_field_qset_data_qualifier_add(unit,
                                                 &qset,
                                                 dq_trill[dq_ndx].qual_id);
      if (BCM_E_NONE != result) {
        print bcm_field_show(unit,"");
        return result;
      }
  }


  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
  /* Don't need to add the action to change the forward header for Jericho */
  if (!is_device_or_above(unit,JERICHO)) {
      BCM_FIELD_ASET_ADD(aset, bcmFieldActionForwardingTypeNew);
  }

  /*
   *  Create the group and set is actions.  This is two distinct API calls due
   *  to historical reasons.  On soc_petra, the action set call is obligatory, and
   *  a group must have at least one action in its action set before any entry
   *  can be created in the group.
   */
  grp = group;
  result = bcm_field_group_create_id(unit, qset, group_priority, grp);
  if (BCM_E_NONE != result) {
    return result;
  }
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }

  /*
   *  Add an entry to the group (could add many entries).
   *
   *  Relatively basic example here, only drop and count.  We could also take
   *  other actions (list them in the aset above).  Some actions only take the
   *  BCM canonical form of param0,param1; other actions (such as redirect and
   *  mirror) can take a GPORT as the port argument; such GPORT can be any
   *  valid GPORT in the given context (redirect family of actions the GPORT
   *  can be anything that works out to be a destination type fwd action, same
   *  for mirror).
   *
   *  This example will match Novell NetWare IPX frames sent using the
   *  Ethernet II frame format by certain Broadcom NICs, and count + drop them.
   */

  /* Don't need to add the terminate Trill header entry for Jericho */
  if (is_device_or_above(unit,JERICHO)) {
      entry_cnt = 1;
  } else {
      entry_cnt = 2;
  }
  for (class_port = 0; class_port < entry_cnt; ++class_port) {
      /* Add two entries. One for recycle port that won't do rpfc check, and one for the others */
      result = bcm_field_entry_create(unit, grp, &ent);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      result = bcm_field_qualify_HeaderFormat(unit, ent, bcmFieldHeaderFormatEthTrillEth);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
      
      /* qualifier for m-bit */
      mbit_data = 0x8;
      mbit_mask = 0x8;
      result = bcm_field_qualify_data(unit,
                                      ent,
                                      dq_trill[0].qual_id,
                                      &mbit_data,
                                      &mbit_mask,
                                      1 /* len here always in bytes */);
      if (BCM_E_NONE != result) {
        print bcm_field_show(unit,"");
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
      
      result= bcm_field_entry_prio_set(unit, ent, class_port);
      if (result != BCM_E_NONE)
      {
        printf("ERROR: bcm_field_entry_prio_set returned %s\n", bcm_errmsg(result));
      }
      /* For recycling entry, set the port in the entry, and don't drop
         For other entries, ignore the port and set action as drop */
      data = class_port;
      if (is_device_or_above(unit,JERICHO) && !is_single_core) {
          mask = 0x2;
      } else {
          mask = 0x1; 
      }
      result = bcm_field_qualify_InterfaceClassPort(unit, ent, data, mask);
      if (result != BCM_E_NONE)
      {
          printf("ERROR: bcm_field_qualify_InterfaceClassPort returned %s\n", bcm_errmsg(result));
      }

      if (class_port == 0) {
          /*
           *  Once we match the frames, let's drop them if not recycle port.
           */
          /* note that param0 and param1 don't care for bcmFieldActionDrop) */
          result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
          if (BCM_E_NONE != result) {
            auxRes = bcm_field_entry_destroy(unit, ent);
            auxRes = bcm_field_group_destroy(unit, grp);
            return result;
          }
      }
      else {
          /* For recycle port, set the Forwarding-Code and Forwading-Offset-Index to be bridged */
          result = bcm_field_action_add(unit, ent, bcmFieldActionForwardingTypeNew, bcmFieldForwardingTypeL2, 3);
          if (BCM_E_NONE != result) {
            auxRes = bcm_field_entry_destroy(unit, ent);
            auxRes = bcm_field_group_destroy(unit, grp);
            return result;
          }
      }
  }

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it's a bit quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_entry_destroy(unit, ent);
    auxRes = bcm_field_stat_destroy(unit, statId);
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }
  
  return result;
}

/* Add TRILL multicast RPF entry */
int trill_multicast_source_add(
                            int unit, 
                            bcm_trill_name_t root_name, 
                            bcm_trill_name_t source_rbridge_name, 
                            bcm_gport_t port, 
                            bcm_if_t encap_intf,
                 /* in */   bcm_field_group_t group,
                 /* in */   int qual_id_ing_nickname,
                 /* in */   int qual_id_eg_nickname) {
  int result;
  bcm_field_group_t grp = group;
  bcm_field_entry_t ent;
  uint8 dq_data[2], dq_mask[2];
  uint64 data64, mask64;
  uint8 data, mask;

    /* Add two entries. One for recycle port that won't do rpfc check, and one for the others */
    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        printf("ERROR: bcm_field_entry_create returned %s\n", bcm_errmsg(result));
      return result;
    }

    /* LEM Lookup found */
    result = bcm_field_qualify_L2SrcHit(unit, ent, 0x1, 0x1);
    if (BCM_E_NONE != result) {
        printf("ERROR: bcm_field_qualify_HeaderFormat returned %s\n", bcm_errmsg(result));
      return result;
    }

    /* LEM Lookup value: bits 14:0 */
    COMPILER_64_SET(data64, 0, (encap_intf<<19));
    COMPILER_64_SET(mask64, 0, 0x7FF80000);
    result = bcm_field_qualify_L2SrcValue(unit, ent, data64, mask64);
    if (BCM_E_NONE != result) {
        printf("ERROR: bcm_field_qualify_HeaderFormat returned %s\n", bcm_errmsg(result));
      return result;
    }

    /* not recycle port  */
    data = 0;
    if (is_device_or_above(unit,JERICHO)) {
        mask = 0x2;
    } else {
        mask = 0x1; 
    }
    result = bcm_field_qualify_InterfaceClassPort(unit, ent, data, mask);
    if (BCM_E_NONE != result) {
        printf("ERROR: bcm_field_qualify_InterfaceClassPort returned %s\n", bcm_errmsg(result));
      return result;
    }

    /* qualifier on ingress nickname */
    dq_data[1] = source_rbridge_name & 0xFF;
    dq_data[0] = (source_rbridge_name >> 8) & 0xFF;
    dq_mask[0] = 0xFF;
    dq_mask[1] = 0xFF;
    result = bcm_field_qualify_data(unit,
                                    ent,
                                    qual_id_ing_nickname,
                                    dq_data,
                                    dq_mask,
                                    2 /* len here always in bytes */);
    if (BCM_E_NONE != result) {
        printf("ERROR: bcm_field_qualify_data returned %s\n", bcm_errmsg(result));
      return result;
    }

    /* qualifier on egress nickname */
    dq_data[1] = root_name & 0xFF;
    dq_data[0] = (root_name >> 8) & 0xFF;
    dq_mask[0] = 0xFF;
    dq_mask[1] = 0xFF;
    result = bcm_field_qualify_data(unit,
                                    ent,
                                    qual_id_eg_nickname,
                                    dq_data,
                                    dq_mask,
                                    2 /* len here always in bytes */);
    if (BCM_E_NONE != result) {
        printf("ERROR: bcm_field_qualify_data2 returned %s\n", bcm_errmsg(result));
      return result;
    }

    result= bcm_field_entry_prio_set(unit, ent, 10); /* higher priority */
    if (result != BCM_E_NONE)
    {
      printf("ERROR: bcm_field_entry_prio_set returned %s\n", bcm_errmsg(result));
    }    

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("ERROR: bcm_field_group_install returned %s\n", bcm_errmsg(result));
      return result;
    }

    return result;

}

int rpf_drop_set_teardown(/* in */ int unit,
                       /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_entry_t ent[128];
  int entCount;
  int entIndex;

  do {
    /* get a bunch of entries in this group */
    result = bcm_field_entry_multi_get(unit, group, 128, &(ent[0]), entCount);
    if (BCM_E_NONE != result) {
      return result;
    }
    for (entIndex = 0; entIndex < entCount; entIndex++) {
      /* remove each entry from hardware and destroy it */
      result = bcm_field_entry_remove(unit, ent[entIndex]);
      if (BCM_E_NONE != result) {
        return result;
      }
      result = bcm_field_entry_destroy(unit, ent[entIndex]);
      if (BCM_E_NONE != result) {
        return result;
      }
    }
    /* all as long as there were entries to remove & destroy */
  } while (entCount > 0);

  /* destroy the group */  
  result = bcm_field_group_destroy(unit, group);

  return result;
}
int
trill_mc_uc_transit(int unit) {
    int rv = 0;
    bcm_if_t encap_id;
    uint32 flags;
    bcm_trill_port_t root_port;
    bcm_l2_addr_t     l2_addr; 
    bcm_field_group_t rpf_group = 100;
    int qual_id_ing_nickname;
    int qual_id_eg_nickname;
    bcm_trill_multicast_entry_t mc_entry;
    bcm_multicast_t l2mc_group_1 = 6262;
    bcm_if_t        host_encap;

    dist_tree_1 = nickname_rb2;

    rv = rpf_drop_set(unit,10, rpf_group, &qual_id_ing_nickname, &qual_id_eg_nickname);
    if (rv != BCM_E_NONE) {
        printf("Error, in rpf_drop_set\n");
        return rv;
    }

     /* Configure TRILL_ALL_RBRIDGES Upper 3 bytes of MAC address to match */ 
    rv = bcm_switch_control_set(unit, bcmSwitchTrillBroadcastDestMacOui, 0x0180C2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_switch_control_set:bcmSwitchTrillBroadcastDestMacOui \n");
        return rv;
    }

    /* Configure TRILL_ALL_RBRIDGES Lower 3 bytes of MAC address to match */
    rv = bcm_switch_control_set(unit, bcmSwitchTrillBroadcastDestMacNonOui, 0x999990);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_switch_control_set:bcmSwitchTrillBroadcastDestMacNonOui \n");
        return rv;
    }

    rv = trill_uni(unit);
    BCM_IF_ERROR_RETURN(rv);
    
    rv = mc_global(unit);
    BCM_IF_ERROR_RETURN(rv);

    rv = create_mc_group(unit, l2mc_group, 1, 1);
    BCM_IF_ERROR_RETURN(rv);

    bcm_trill_multicast_entry_t_init(&mc_entry);
    mc_entry.root_name = nickname_rb2;
    mc_entry.c_vlan = vid_1;
    mc_entry.group = l2mc_group;
    rv = bcm_trill_multicast_entry_add(unit, &mc_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Add RPF-Check entry to the TCAM */
     rv = bcm_multicast_trill_encap_get(unit, l2mc_group, trill_port_rb1.trill_port_id ,0, &encap_id); 
    BCM_IF_ERROR_RETURN(rv);
    printf ("TRILL encap for RPF check (Adj-eep)= %d \n", encap_id);
    
    rv = trill_multicast_source_add(unit, nickname_rb2, nickname_rb1, port_1,encap_id, rpf_group,
                                         qual_id_ing_nickname, qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);

            /* Create Multicast Group */  
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 |  BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &l2mc_group_1);
    BCM_IF_ERROR_RETURN(rv);
    printf ("TRILL l2mc_group = %d \n", l2mc_group_1);
    
  
        /* Get trill encapsulation */   
    rv = bcm_multicast_trill_encap_get(unit, l2mc_group_1, trill_port_rb2.trill_port_id ,0, &encap_id); 
    BCM_IF_ERROR_RETURN(rv);
    printf ("TRILL encap = %d \n", encap_id);

    rv = bcm_multicast_egress_add(unit, l2mc_group_1, port_3, encap_id);
    BCM_IF_ERROR_RETURN(rv);
   
   
    
    
    /* Add port + encapsulation to MC group */
    rv = bcm_multicast_egress_add(unit, l2mc_group_1, port_3, host_encap);
    BCM_IF_ERROR_RETURN(rv);
    
    return rv;
}

int
test_trill_uni(int unit, int p1, int p2, int trill_ether_type,int hop) {
    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    
    trill_uni(unit);
}
int
test_trill_mc_transit_fecless(int unit, int p1, int p2, int trill_ether_type,int hop, bcm_multicast_t group) {
    printf("temp group: 0x%x", group); 
    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    l2mc_group = group;
    
    trill_mc_transit_fecless(unit);
}
int
test_trill_mc_uc_transit(int unit, int p1, int p2, int p3, int trill_ether_type,int hop, bcm_multicast_t group) {
    port_1 = p1;
    port_2 = p2;
    port_3 = p3;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    l2mc_group = group;
    
    trill_mc_uc_transit(unit);
}

int
test_trill_mc_ingress_ecmp(int unit, int p1, int p2, int p3, int p4, int trill_ether_type,int hop, bcm_multicast_t group) {
    port_1  =  p1;
    port_2  = p2;
    port_3  = p3;
    port_4  = p4;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    l2mc_group = group;
    
    trill_mc_ingress_ecmp(unit);

}

int
test_clear_trill_uni(int unit) {
    int rv = BCM_E_NONE;
    bcm_vlan_t
        vsi_1_bridge,
        vsi_2_desig;
 
    bcm_l2_addr_t               l2_addr; 
    bcm_trill_port_t            trill_port;

    /* For Jericho, we use the FGL to configure the VL */
    if (is_device_or_above(unit, JERICHO)) {
        rv = test_clear_trill_uni_fgl(unit);
        return rv;
    }
 
    rv = vlan_term(unit, vid_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_term\n");
        return rv;
    }

    vsi_1_bridge = vid_1;
    
    rv = vlan_port_term(unit, vid_1, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_term\n");
        return rv;
    }

    
    rv = l3_intf_delete(unit, l3_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3_intf_delete\n");
        return rv;
    }
    

    rv = vlan_term(unit, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_term\n");
        return rv;
    }
    vsi_2_desig = vid_2_desig;        
            
    rv = vlan_port_unset(unit, port_1, vid_1, vsi_1_bridge);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_unset: vid=$vid, vsi=$vsi\n");
        return rv;
    }
     
    rv = vlan_port_unset(unit, port_1, vid_2_desig, vsi_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_unset: vid=$vid, vsi=$vsi\n");
        return rv;
    }

    rv = vlan_port_term(unit, vid_2_desig, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_term(unit, vid_1, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_term(unit, vid_2_desig, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }  
    rv = remove_local_rbridge(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in remove_local_rbridge\n");
        return rv;
    } 

    printf("Deleted local trill port: nickname %d\n", nickname_rb_local );

    /* remove rb1 multicast adjacency entry */
    rv = mult_adjacency(unit,trill_port_rb1.trill_port_id, FALSE);

    if (rv != BCM_E_NONE) {
        printf("Error, in mult_adjacency, \n");
        return rv;
    }

    rv = remove_remote_rbridge(unit, trill_port_rb1.trill_port_id );
    if (rv != BCM_E_NONE) {
        printf("Error, in remove_remote_rbridge\n");
        return rv;
    }

    printf("Deleted trill port: id=%d nickname %d\n", trill_port_rb1.trill_port_id, nickname_rb1 );
    trill_port_rb1.trill_port_id = -1;

    /* remove rb2 multicast adjacency entry */
    rv = mult_adjacency(unit,trill_port_rb2.trill_port_id, FALSE);

    if (rv != BCM_E_NONE) {
        printf("Error, in mult_adjacency, \n");
        return rv;
    }

    rv = remove_remote_rbridge(unit, trill_port_rb2.trill_port_id );
    if (rv != BCM_E_NONE) {
        printf("Error, in remove_remote_rbridge\n");
        return rv;
    }

    printf("Deleted trill port: id=%d nickname=%d\n", trill_port_rb2.trill_port_id, nickname_rb2 );
    trill_port_rb2.trill_port_id = -1; 


    rv = port_config_reset(unit, port_1, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_reset\n");
        return rv;
    }
    printf("Reset config for port = %d\n", port_1 );
    rv = port_config_reset(unit, port_2, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_reset\n");
        return rv;
    }
    printf("Reset config for port = %d\n", port_2 );
    rv = traps_unset(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in traps_unset\n");
        return rv;
    }



   trill_uni_configured = FALSE;

    return rv;
}
int
test_clear_trill_mc_transit_fecless_config(int unit, int is_fgl, int double_tag, int is_2pass) {
    int rv = BCM_E_NONE;
    bcm_trill_multicast_entry_t mc_entry;
    bcm_trill_port_t            trill_port;
    bcm_if_t                    encap_id;
    bcm_if_t                    host_encap;
    bcm_field_group_t           rpf_group = 100;

    if (trill_bridge_mode) {
        /* Destroy Multicast Group */  
        rv = remove_transit_mc_group(unit, trill_mc_group, 1, !is_2pass);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Destroy Multicast Group */  
    rv = remove_mc_group(unit, l2mc_group, 1, !is_2pass);
    BCM_IF_ERROR_RETURN(rv);

    if (mc_id) {

        bcm_trill_multicast_entry_t_init(&mc_entry); 
      
        mc_entry.root_name = dist_tree_1;
        if (is_fgl) {
         mc_entry.c_vlan = FGL_outer_vid;
         if (double_tag) {
             mc_entry.c_vlan_inner = FGL_inner_vid;
         } else {
             mc_entry.c_vlan_inner = BCM_VLAN_INVALID; 
         }
        } else {
            mc_entry.c_vlan = vid_1;
        }
        mc_entry.group = (trill_bridge_mode ? trill_mc_group : l2mc_group);
        rv = bcm_trill_multicast_entry_delete(unit, &mc_entry);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (trill_bridge_mode) {
        rv = remove_root_rbridge(unit, dist_tree_1_port_nb[0].trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = remove_root_rbridge(unit, dist_tree_1_port_nb[1].trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = remove_root_rbridge(unit, dist_tree_1_port.trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (trill_bridge_mode) {
        rv = remove_root_rbridge(unit, dist_tree_2_port_nb[0].trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = remove_root_rbridge(unit, dist_tree_2_port_nb[1].trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = remove_root_rbridge(unit, dist_tree_2_port.trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
    }
    /* old implementation that configure sw db (mc-id to nickname)*/
    if (mc_id) {
    
        bcm_trill_multicast_entry_t_init(&mc_entry); 
        mc_entry.root_name = dist_tree_3;
        mc_entry.c_vlan = vid_1;
        mc_entry.group = dummy_mc_group;
        rv = bcm_trill_multicast_entry_delete(unit, &mc_entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (trill_bridge_mode) {
        rv = remove_root_rbridge(unit, dist_tree_3_port_nb[0].trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = remove_root_rbridge(unit, dist_tree_3_port_nb[1].trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = remove_root_rbridge(unit, dist_tree_3_port.trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = bcm_multicast_destroy(unit, dummy_mc_group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("TRILL dummy_mc_group = %d \n", dummy_mc_group);

    rv = mc_global_reset(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (is_fgl) {
        rv = test_clear_trill_uni_fgl(unit);
    } else {
        rv = test_clear_trill_uni(unit);
    }
    if (rv != BCM_E_NONE) {
        printf("Error, in test_clear_trill_uni\n");
        return rv;
    }

    rv = bcm_l2_addr_delete_by_vlan(unit,vid_1,0);
    BCM_IF_ERROR_RETURN(rv);

    rv = rpf_drop_set_teardown(unit, rpf_group);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}
int
test_clear_trill_mc_uc_transit(int unit) {
    int rv = BCM_E_NONE;
    bcm_vlan_t
        vsi_1_bridge,
        vsi_2_desig;
 
    bcm_l2_addr_t               l2_addr; 
    bcm_trill_port_t            trill_port;
    bcm_trill_multicast_entry_t mc_entry;
 
    bcm_trill_multicast_entry_t_init(&mc_entry);
    mc_entry.root_name = nickname_rb2;
    mc_entry.c_vlan = vid_1;
    mc_entry.group = l2mc_group;
    rv = bcm_trill_multicast_entry_delete(unit, &mc_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_multicast_destroy(unit,l2mc_group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Destroy MC group = %d \n", l2mc_group);

    rv = test_clear_trill_uni(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_term\n");
        return rv;
    }

    return rv;

}
int
test_clear_trill_mc_transit_fecless(int unit) {
    return test_clear_trill_mc_transit_fecless_config(unit,0,0,0);
}
int
test_clear_trill_mc_transit_fecless_fgl(int unit, int double_tag) {
    return test_clear_trill_mc_transit_fecless_config(unit,1,double_tag,0);
}

int
test_clear_trill_ecmp(int unit) {

    int i;
    uint32              rv = 0;

    for (i = 0; i < ecmp_mc_ing_nof_ifs; ++i) {
        rv = remove_root_rbridge(unit, ecmp_mc_trill_ports[i].trill_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in remove_root_rbridge\n");
            return rv;
        }
        rv = bcm_multicast_destroy(unit,ecmp_mc_ing_mc_groups[i]);
        BCM_IF_ERROR_RETURN(rv);
        printf ("Destroy MC group = %d \n", ecmp_mc_ing_mc_groups[i]);

    }
    /* old implementation delete ecmp port */
    if (create_ecmp_port) {
        rv = bcm_trill_port_delete(unit, ecmp_trill_port.trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
        printf("Deleted ecmp trill port %d\n", ecmp_trill_port.trill_port_id );
    }

    if (cint_trill_cfg.ecmp_api_is_ecmp) {
      rv = bcm_l3_egress_ecmp_destroy(unit, ecmp);
      BCM_IF_ERROR_RETURN(rv);
      printf("Deleted multipath interface %d\n", ecmp.ecmp_intf);
    } else {
      rv = bcm_l3_egress_ecmp_destroy(unit, ecmp_mpintf);
      BCM_IF_ERROR_RETURN(rv);
      printf("Deleted multipath interface %d\n", ecmp_mpintf);
    }
    
    rv = bcm_l2_addr_delete(unit, c_dmac, vid_1);
    if (rv != BCM_E_NOT_FOUND) {
        BCM_IF_ERROR_RETURN(rv);
    }

    return  test_clear_trill_uni(unit);
}

int
trill_uni_fgl(int unit, int double_tag) {

    int rv = BCM_E_NONE;
    int vsi1 =vid_1;
    bcm_trill_vpn_config_t              vpn_config;
    bcm_vlan_action_set_t               action;
    bcm_l2_addr_t                       l2_addr;
    bcm_vlan_t  vsi_2_desig;

    vsi_2_desig = vid_2_desig; 

    if (trill_uni_configured_fgl) {
        return rv;
    }
    printf("trill_uni_FGL %d %d\n",port_1, port_2);
    /* Initialize trill module (SW related databases) */
    rv = bcm_trill_init(unit);
    if ((rv != BCM_E_NONE ) &&( rv != BCM_E_INIT)) {
        printf("Error, in bcm_trill_init\n");
        return rv;
    }    

    /* Global config:
       - update inLif to Trill inLif using PMF  
       - Hop count
       - EtherType
       - Acceptable frame type
       - Port -> Vlan domain
       - Designated Vlan 
        */
    rv = global_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }
    rv = port_config(unit, port_1, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_1\n");
        return rv;
    }
    
    rv = port_config_FGL(unit, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_FGL: port_1\n");
        return rv;
    }

    /* In advanced vlan translation mode, default vlan translation actions are not created in the device.
       We call this function to compensate. If the device is in normal vlan translation mode, it does nothing. */
    rv = trill_advanced_vlan_translation_actions_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in trill_advanced_vlan_translation_actions_init\n");
        return rv;
    }

    rv = port_config(unit, port_2, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_2\n");
        return rv;
    }    

    rv = port_config_FGL(unit, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_FGL: port_2\n");
        return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vsi1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_create_with_id\n");
        return rv;
    } 
    printf("Created vswitch id %d\n",vsi1);

    rv = vlan_init(unit, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vsi1, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_init\n");
        return rv;
    }
    rv = vlan_port_init(unit, vid_2_desig, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vid_2_desig, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }  
    vpn_config.vpn = vsi1;
    if ((double_tag)) {
        vpn_config.high_vid = FGL_outer_vid;
        vpn_config.low_vid = FGL_inner_vid;
    } else {
        vpn_config.high_vid = vid_1;
        vpn_config.low_vid = BCM_VLAN_INVALID;
    }

    vpn_config.flags = BCM_BCM_TRILL_VPN_NETWORK | BCM_BCM_TRILL_VPN_ACCESS;
    
    rv = bcm_trill_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_trill_vpn_create\n");
       print vpn_config;
       return rv;
    }
    printf("Created VPN id %d , high_vid=%d, low_vid=%d\n",vsi1, vpn_config.high_vid, vpn_config.low_vid);
    
    rv = vlan_port_set(unit, port_1, vsi1, vsi1); 
    if (rv != BCM_E_NONE) {
       printf("Error, in vlan_port_set: vsi=%d\n",vsi1 );
       return rv;
    }

    rv = l3_intf_create(unit, trill_vsi_mac_2, vsi_2_desig, &l3_intf_id);
    if (rv != BCM_E_NONE) {
       printf("Error, in l3_intf_create: vsi=%d\n",vsi_2_desig );
       return rv;
    }

    /* Add TRILL Virtual Port for local RB (configures my nickname) */
    rv = config_local_rbridge(unit, nickname_rb_local, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in config_local_rbridge: vsi=$vsi_1_bridge\n");
        return rv;
    }
    printf("configured local bridge %d\n", nickname_rb_local );

    rv = add_remote_rbridge(unit, remote_mac_rb1, vsi_2_desig, port_1, nickname_rb1, 0, &trill_port_rb1);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi1);
        return rv;
    }
    
    rv = add_remote_rbridge(unit, remote_mac_rb2, vsi_2_desig, port_2, nickname_rb2, 0, &trill_port_rb2);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi1 );
        return rv;
    }
       
    /* Add L2 entry for Customer packet to TRILL tunnel:
       Adds an entry to the LEM with <c_dmac, nickname (taken from trill_port)> -> FEC */
    bcm_l2_addr_t_init(&l2_addr, c_dmac, vsi1);
    if (!trill_bridge_mode) {
        l2_addr.port = trill_port_rb1.trill_port_id;
    } else {
        BCM_GPORT_FORWARD_PORT_SET(l2_addr.port, trill_port_rb1.egress_if);
    }       

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    if(trill_bridge_mode) {        
        /* For Transit Trill packets */
        rv = add_transit_rbridge(unit, port_1, nickname_rb1, trill_port_rb1.trill_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in trill_nb_add_transit_rbridge: nickname=%d\n", nickname_rb2);
            return rv;
        } 
        
        /* For Transit Trill packets */
        rv = add_transit_rbridge(unit, port_2, nickname_rb2, trill_port_rb2.trill_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in trill_nb_add_transit_rbridge: nickname=%d\n", nickname_rb2);
            return rv;
        } 
    }

    /* Add L2 entry for Customer packet to TRILL tunnel: for egress rb flow
       Adds an entry to the LEM with <da= host1_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host1_mac, vid_1);
    l2_addr.port = port_1;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }


    /* Add L2 entry for Customer packet to TRILL tunnel: for bridge flow
       Adds an entry to the LEM with <da= host4_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host4_mac, vid_1);
    l2_addr.port = port_2;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }
    trill_uni_configured_fgl = TRUE;

    return rv;


}
int
trill_mc_transit_fecless_2pass(int unit, int is_ptch, int is_fgl, int double_tag) {
    int rv = 0;
    int recycle_port_num = 1;
    int is_single_core = 0;

    is_single_core = (SOC_DPP_DEFS_GET_NOF_CORES(unit) > 1) ? FALSE : TRUE;
    if (is_device_or_above(unit, JERICHO) && (!is_single_core)) {
        recycle_port_num = 2;
    }

    rv = port_config_recycle_port(unit, recycle_port, recycle_port_num,
            &port_1, 1, vid_2_desig, snoop_trap_gport_id, &snoop_group, is_ptch);
    BCM_IF_ERROR_RETURN(rv);

    rv =  trill_mc_transit_fecless_config(unit, is_fgl, double_tag, 1); 
    BCM_IF_ERROR_RETURN(rv);

    rv = create_flooding_port(unit, vid_1, flood_mc_group, flood_nick, &flood_trill_port);
    BCM_IF_ERROR_RETURN(rv);

    /* Add L2 entries for the second pass */

    rv = trill_mc_transit_fecless_2pass_config_second_pass(unit);
    BCM_IF_ERROR_RETURN(rv);

   return rv;
}
int
trill_mc_transit_fecless_2pass_config_second_pass(int unit) {
    int rv=0;
    int mc_group =  6700;
    bcm_l2_addr_t   l2_addr; 
    bcm_if_t        host_encap;

    bcm_l2_addr_t_init(&l2_addr, host3_mac, vid_1);
    l2_addr.port = port_2;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }
    rv = create_mc_group(unit,mc_group, 0, 1);
    BCM_IF_ERROR_RETURN(rv);

    host_encap = get_encap_id_to_bridge(unit);

    /* Add one more port to group*/
    rv = bcm_multicast_egress_add(unit, mc_group, port_3, host_encap);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_engress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group, port_1, encap_id);
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, host4_mac, vid_1);
    l2_addr.l2mc_group = mc_group;
    l2_addr.port = dist_tree_1_port.trill_port_id;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

   return rv;
}
int
trill_mc_transit_fecless_2pass_unconfig_second_pass(int unit) {
    int rv=0;
    int mc_group =  6700;

    rv = bcm_l2_addr_delete(unit, host3_mac, vid_1);
    if (rv != BCM_E_NOT_FOUND) {
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = bcm_l2_addr_delete(unit, host4_mac, vid_1);
    if (rv != BCM_E_NOT_FOUND) {
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = bcm_multicast_destroy(unit, mc_group);
    BCM_IF_ERROR_RETURN(rv);

    return rv;

}
int
test_trill_uni_fgl(int unit, int p1, int p2, int trill_ether_type,int hop, int double_tag) {
    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;

    trill_uni_fgl(unit, double_tag);

}
int
test_trill_mc_transit_fecless_fgl(int unit, int p1, int p2, int trill_ether_type,int hop, bcm_multicast_t group, int double_tag, int is_2pass) {
    int rv = BCM_E_NONE;

    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    l2mc_group = group;

    trill_mc_transit_fecless_config(unit, 1, double_tag, 0);
}
int
test_trill_mc_transit_fecless_2pass(int unit, int p1, int p2, int p3, int trill_ether_type,int hop,
                                    bcm_multicast_t group, int rcy_port, int rcy_port_core1, int is_ptch,
                                    int is_fgl, int double_tag) {
    int rv = BCM_E_NONE;

    port_1 = p1;
    port_2 = p2;
    port_3 = p3;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    l2mc_group = group;
    recycle_port[0] = rcy_port;
    recycle_port[1] = rcy_port_core1;

    rv = trill_mc_transit_fecless_2pass(unit, is_ptch, is_fgl, double_tag);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

int
test_clear_trill_mc_transit_fecless_2pass(int unit,  int is_fgl, int double_tag) {
    int rv = BCM_E_NONE;
    int i;
    int recycle_port_num = 1;
    bcm_trill_multicast_entry_t mc_entry;
    bcm_trill_port_t            trill_port;
    bcm_if_t                    encap_id;
    int is_single_core = 0;

    is_single_core = (SOC_DPP_DEFS_GET_NOF_CORES(unit) > 1) ? FALSE : TRUE;

    rv = test_clear_trill_mc_transit_fecless_config(unit, is_fgl, double_tag, 1); 
    BCM_IF_ERROR_RETURN(rv);


    if (is_device_or_above(unit, JERICHO) && (!is_single_core)) {
        recycle_port_num = 2;
    }
    for (i = 0; i < recycle_port_num; i++) {
        rv = port_config_reset(unit, recycle_port[i], vid_2_desig);
        if (rv != BCM_E_NONE) {
            printf("Error, in port_config_reset\n");
            return rv;
        }
    }
    rv = trill_2pass_field_group_unset(unit, snoop_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in trill_2pass_field_group_unset\n");
        return rv;
    }

    rv = two_pass_snoop_unset(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in two_pass_snoop_unset\n");
        return rv;

    }
    rv = remove_flooding_port(unit, vid_1, flood_mc_group, flood_nick, flood_trill_port.trill_port_id);
    BCM_IF_ERROR_RETURN(rv);

    rv = trill_mc_transit_fecless_2pass_unconfig_second_pass(unit);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}
int
test_clear_trill_uni_fgl(int unit) {

   int rv = BCM_E_NONE;
    bcm_vlan_t
        vsi_1_bridge,
        vsi_2_desig;

    bcm_l2_addr_t               l2_addr;
    int                         vsi1=vid_1;
    vsi_1_bridge = vid_1;
    rv = l3_intf_delete(unit, l3_intf_id);
    BCM_IF_ERROR_RETURN(rv);
    printf("deleted l3_intf %d\n", l3_intf_id);

    rv = vlan_term(unit, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);
    printf("vlan_term  %d\n", vid_2_desig);

    vsi_2_desig = vid_2_desig;        
           
    rv = vlan_port_unset(unit, port_1, vid_1, vid_1);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_2_desig, port_1);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_1, port_2);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_2_desig, port_2);
    BCM_IF_ERROR_RETURN(rv);

    rv = remove_local_rbridge(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (trill_bridge_mode) {
        rv = remove_transit_rbridge(unit, nickname_rb1, trill_port_rb1.trill_port_id);
        BCM_IF_ERROR_RETURN(rv);

        rv = remove_transit_rbridge(unit, nickname_rb2, trill_port_rb2.trill_port_id);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = remove_remote_rbridge(unit, trill_port_rb1.trill_port_id);
    BCM_IF_ERROR_RETURN(rv);
    trill_port_rb1.trill_port_id= -1;

    rv = remove_remote_rbridge(unit, trill_port_rb2.trill_port_id);
    BCM_IF_ERROR_RETURN(rv);
    trill_port_rb2.trill_port_id= -1;

    rv = port_config_reset(unit, port_1, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);

    printf("Reset config for port = %d\n", port_1 );
    rv = port_config_reset(unit, port_2, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);

    printf("Reset config for port = %d\n", port_2 );
    rv = traps_unset(unit);
    BCM_IF_ERROR_RETURN(rv);


    rv =  bcm_vswitch_destroy(unit, vsi1);
    BCM_IF_ERROR_RETURN(rv);

    printf("vswitch removed\n" );

    rv=bcm_trill_vpn_destroy(unit, vsi1);
    BCM_IF_ERROR_RETURN(rv);

    printf("vpn removed\n" );

    /* In advanced vlan translation mode, we create vlan translation actions that are not created by default.
       We call this to destroy them. If advanced vlan translation mode is off, it does nothing.*/
    rv = vlan_translation_default_mode_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_default_mode_destroy\n");
        return rv;
    }

    printf("Destroyed vlan translation actions\n");

    trill_uni_configured_fgl = FALSE;
    return rv;
}

/* test function that remove rb1 and rb2 adjacency entries*/
int test_remove_multicast_adjacency(int unit)
{
    int rv = BCM_E_NONE;

    rv = mult_adjacency(unit, trill_port_rb1.trill_port_id, FALSE );
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted multicast adjacency on port = %d \n", trill_port_rb1.trill_port_id );    

    rv = mult_adjacency(unit, trill_port_rb2.trill_port_id, FALSE );
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted multicast adjacency on port = %d \n", trill_port_rb2.trill_port_id );

    return rv;

}
int set_same_interface_filter(int unit, int port, int enabled){
    int rv = 0;
    bcm_rx_trap_config_t config;
    int flags = 0;
    
    rv = bcm_port_control_set(unit, port, bcmPortControlBridge, enabled);
    if ((rv != BCM_E_NONE)) {
        printf("Error, bcm_port_control_set failed: %d\n", rv);
        return rv;
    } else if (enabled){
        printf("Same interface filter is enabled successfully\n");
    } else {
        printf("Same interface filter is disabled successfully\n");   
    }

    if (!is_same_interface_filter_enabled) {
        bcm_rx_trap_config_t_init(&config);
        bcm_rx_trap_t trap = bcmRxTrapSameInterface;
        int tid;

        config.flags = (BCM_RX_TRAP_UPDATE_DEST); 
        if (enabled) {
            config.trap_strength = 7;
            config.dest_port = BCM_GPORT_BLACK_HOLE;
        } else {
            config.trap_strength = 0;
        }


        rv = bcm_rx_trap_type_create(unit,flags,trap,&tid);
        BCM_IF_ERROR_RETURN(rv);
        trap_id_same_interface = tid;

        bcm_rx_trap_set(unit,tid,&config);
        BCM_IF_ERROR_RETURN(rv);

        is_same_interface_filter_enabled = TRUE;
    }

    return rv;
}
/*  This is an example for unicast trill with transit ECMP */
/*  The ECMP contains 3 rbridges (two defined by trill_uni and one more added here for better demonstration of the behavior.) */

int
trill_uni_with_transit_ecmp(int unit) {
    const int           nof_ifs = 3;
    bcm_if_t            eg_ifs[nof_ifs];
    bcm_l3_egress_ecmp_t  ecmp_local;
    bcm_if_t              ecmp_mpintf_local;
    int rv;
    bcm_mac_t           remote_mac_rb3 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};

    bcm_trill_port_t    trill_port_rb3;
        
    
    int     nickname_rb3  = 0xAA33;

    rv = trill_uni(unit);
    BCM_IF_ERROR_RETURN(rv);

    rv = port_config(unit, port_3, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_2\n");
        return rv;
    }
    rv = add_remote_rbridge(unit, remote_mac_rb3, vid_2_desig, port_3, nickname_rb3, 0, &trill_port_rb3);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi_2_desig );
        return rv;
    }

    eg_ifs[0] = trill_port_rb1.egress_if;
    eg_ifs[1] = trill_port_rb2.egress_if;
    eg_ifs[2] = trill_port_rb3.egress_if;

    bcm_l3_egress_ecmp_t_init(&ecmp_local);
    ecmp_local.max_paths = nofs_ifs;

    if (cint_trill_cfg.ecmp_api_is_ecmp) {
      rv = bcm_l3_egress_ecmp_create(unit, &ecmp_local, nof_ifs, eg_ifs);
      BCM_IF_ERROR_RETURN(rv);
      printf("Created multipath interface %d\n", ecmp_local.ecmp_intf);
      ecmp_mpintf_local = ecmp_local.ecmp_intf;
    } else {
      rv = bcm_l3_egress_ecmp_create(unit, &ecmp_mpintf_local, nof_ifs, eg_ifs);
      BCM_IF_ERROR_RETURN(rv);
      printf("Created multipath interface %d\n", ecmp_mpintf_local);
    }
    
    /* Create ecmp trill port ( eg_if=ecmp_eg_if) */
    bcm_trill_port_t ecmp_trill_port;
    bcm_trill_port_t_init(&ecmp_trill_port);

    ecmp_trill_port.name =0xa1a1;
    ecmp_trill_port.egress_if = ecmp_mpintf_local;
    rv = bcm_trill_port_add(unit, &ecmp_trill_port);
    BCM_IF_ERROR_RETURN(rv);
    printf("Created ecmp trill port \n" );
    
    set_same_interface_filter(unit, port_1, 0);
    set_same_interface_filter(unit, port_2, 0);

    rv = bcm_switch_control_set(unit,bcmSwitchECMPHashConfig,BCM_HASH_CONFIG_ROUND_ROBIN);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

int
trill_with_two_vlan_flooding(int unit) {
    int                 rv;
    bcm_trill_port_t    flooding_port_1,flooding_port_2;

    int vlan_1 = 100;
    int vlan_2 = 110;


    rv = trill_mc_transit_fecless_config(unit,0, 0, 0);
    BCM_IF_ERROR_RETURN(rv);

    rv = create_flooding_port(unit, vlan_1, 6900, dist_tree_1, &flooding_port_1);
    BCM_IF_ERROR_RETURN(rv);
    rv = create_flooding_port(unit, vlan_2, 6901, dist_tree_1, &flooding_port_2);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

int
test_trill_nickname_equal_port_id(int unit, int port) {
    int rv = 0;
    create_l3_egress_s egr_obj_rb;
    bcm_if_t egr_if_id_rb;
    bcm_trill_port_t trill_port;
    int trill_port_id;
    int trill_nickname = 1;
    int port_id;

    sal_memcpy(egr_obj_rb.next_hop_mac_addr, remote_mac_rb1, 6);
    egr_obj_rb.vlan = 1; /* VSI < 4K */
    egr_obj_rb.out_gport = port;
    egr_obj_rb.l3_flags = BCM_L3_TRILL_ONLY;

    rv = l3__egress__create(unit,&egr_obj_rb);
    if (rv != BCM_E_NONE) {
        printf("Error in l3__egress__create\n");
        return rv;
    }
    egr_if_id_rb = egr_obj_rb.fec_id;
    printf("Created egress obj: 0x%x \n", egr_if_id_rb);
    

    bcm_trill_port_t_init(&trill_port);
    trill_port.egress_if = egr_if_id_rb;
    trill_port.name = trill_nickname;

    rv = bcm_trill_port_add(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_trill_port_add\n");
        return rv;
    }

    port_id = BCM_GPORT_TRILL_PORT_ID_GET(trill_port.trill_port_id);
    if (port_id != trill_nickname) {
        rv = bcm_trill_port_delete(unit, trill_port.trill_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_trill_port_delete %d\n", trill_port_id);
            return rv;
        }

        /*make sure port id == nick name*/
        trill_nickname = port_id;
        trill_port.egress_if = egr_if_id_rb;
        trill_port.name = trill_nickname;
        trill_port.flags |= BCM_L3_TRILL_ONLY;

        rv = bcm_trill_port_add(unit, &trill_port);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_trill_port_add\n");
            return rv;
        }
    }

    printf("Created trill_port: 0x%x \n", trill_port.trill_port_id);

    trill_port_id = trill_port.trill_port_id;

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  trill_port_id;
    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_trill_port_get trill port id: 0x%x\n", trill_port_id);
        return rv;
    }

    bcm_trill_port_t_init(&trill_port);
    trill_port.name = trill_nickname;
    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_trill_port_get nick name: 0x%x\n", trill_nickname);
        return rv;
    }

    rv = bcm_trill_port_delete(unit, trill_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in  bcm_trill_port_delete trill port id: 0x%x\n", trill_port_id);
        return rv;
    }

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id =  trill_port_id;
    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NOT_FOUND) {
        printf("Error in trill port 0x%x get \n", trill_port_id);

        if (rv == BCM_E_NONE) {
            return BCM_E_FAIL;
        }
        return rv;
    }

    bcm_trill_port_t_init(&trill_port);
    trill_port.name = trill_nickname;
    rv = bcm_trill_port_get(unit, &trill_port);
    if (rv != BCM_E_NOT_FOUND) {
        printf("Error in trill name 0x%x get\n", trill_nickname);

        if (rv == BCM_E_NONE) {
            return BCM_E_FAIL;
        }
        return rv;
    }

    rv = bcm_l3_egress_destroy(unit, egr_if_id_rb);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l3_egress_destroy\n");
        return rv;
    }

    return BCM_E_NONE;
}

int
test_trill_uni_wide_lif(int unit, int p1, int p2, int trill_ether_type,int hop) {
    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    is_wide_lif = 1;

    int rv = BCM_E_NONE;

    rv = trill_uni_fgl(unit, 0);
    return rv;
}

int
set_trill_lif_wide_data (int unit, int wide_data) {
    uint64 wd_data;
    int rv = BCM_E_NONE;
    COMPILER_64_SET(wd_data, 0, wide_data);
    rv = bcm_port_wide_data_set(unit, trill_port_rb1.trill_port_id, BCM_PORT_WIDE_DATA_INGRESS, wd_data);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_wide_data_set\n");
        return rv;
    }

    rv = bcm_port_wide_data_set(unit, trill_port_rb2.trill_port_id, BCM_PORT_WIDE_DATA_INGRESS, wd_data);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_wide_data_set\n");
        return rv;
    }

    return rv;
}

