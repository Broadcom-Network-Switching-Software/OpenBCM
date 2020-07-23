/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* 
 * Cint file to configure vpws protection feature in scenario of interworking with NP.
 */

/*
 *  SOC config:
 *		custom_feature_conn_to_np_enable=1
 *		custom_feature_conn_to_np_debug=0
 *		bcm886xx_vlan_translate_mode=1
 *		ftmh_dsp_extension_add.BCM88675=1
 *		system_ftmh_load_balancing_ext_mode.BCM88675=ENABLED
 *		field_class_id_size_0.BCM88675=8
 *		field_class_id_size_2.BCM88675=24
 *
*        ucode_port_13.BCM88675=10GBase-R64.0:core_0.13
*        ucode_port_14.BCM88675=10GBase-R65.0:core_0.14
*        ucode_port_15.BCM88675=10GBase-R68.0:core_0.15
*        ucode_port_16.BCM88675=10GBase-R69.0:core_0.16
*        ucode_port_32.BCM88675=10GBase-R65.1:core_0.32
*        ucode_port_33.BCM88675=10GBase-R65.2:core_0.33
*        ucode_port_34.BCM88675=10GBase-R65.3:core_0.34
*        ucode_port_35.BCM88675=10GBase-R65.4:core_0.35 

*
 *
 * To Activate Above Settings Run:
 *      cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c 
 *      cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c 
 *      cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
 *      cint ../../../../src/examples/dpp/utility/cint_utils_mpls_port.c 
 *      cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c  
 *      cint ../../../../src/examples/dpp/cint_port_tpid.c 
 *      cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c 
 *      cint ../../../../src/examples/dpp/cint_vswitch_cross_connect_p2p.c  
 *      cint ../../../../src/examples/dpp/cint_nph_vpws_egress.c
 * 
 *  1.   Brief :
 *   The whole configuration makes up 2 parts: ingress part and egress part.
 *   Ingress part: configuration on the AC located chip;
 *   Egress part: congiguration on the PW located chip;
 *   in this example, AC and PW located on the same chip.
 * 
 *   Ingress parts:
 *     a.   Add eedb entry for the pw ZTMH/NPH header;
 *     b.   Configure FEC to implement the protection of PW;
 *     c.   create AC port;
 *     d.  configure the vpws p2p service, conect AC port to PW FEC
 *
 *
 *   Egress parts:
 *     a. configure the PW port;
 *    b. configure the mid AC port for every PW(mid AC port means that "Flowpoint + PS" 2-tuple, egress chip will map it to the final pw service)
 *    c. configure P2P service, connect the mid-AC port to PW with 1to1 relationship  

 *  example:


 *  protection_vpws_config_with_default(0,0);


 *  
 *  traffic : 
 * Access side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                 tag2                           |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TIPD1 | Prio |     Data    |   |
 *   |   |    |    || 0x8100|      | 10  |            |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 3 : Packets Received/Transmitted on Access Port   |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

 * Primary path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |20||Label:41|Label:21||Lable:41||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *
 * Protected path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |20||Label:40|Label:20||Lable:40||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

*/

int verbose1 = 1;
int tunnel_id;
nph_service_info_egress_s g_nph_service_egress_info[4];
struct protection_dataentry_info_s{
    int primary0_pw_eedb_index;
    int primary1_pw_eedb_index;
    int protected0_pw_eedb_index;
    int protected1_pw_eedb_index;

    int ac_eedb_index;


    int pw_fp;
    int pripw_outport;
    int backuppw_outport;
    int pripw_outslot;
    int backuppw_outslot;
    int pripw_flowid;
    int backuppw_flowid;

    int ac_fp;
    int ac_outport;
    int ac_outslot;
    int ac_flowid;

    int lsp_fp;
    int primarylsp_outport;
    int backuplsp_outport;
    int primarylsp_outslot;
    int backuplsp_outslot;
    int primarylsp_flowid;
    int backuplsp_flowid;

    bcm_gport_t primary_lsp_egress_ac;
    bcm_gport_t backup_lsp_egress_ac;

    int primarylsp_eedb_index;
    int backuplsp_eedb_index;

    int lsr_ecmp_eedb_index[4];
    bcm_if_t lsr_ecmp_fec_index[4];

    bcm_gport_t lsp_ecmp_egress_ac[4];

};

protection_dataentry_info_s pro_data_entry_info;

struct mpls_info_t{
    int vpn_id;
    bcm_gport_t access_port_id;
    int        access_port_in_vlan;
    bcm_gport_t ingress_mpls_port_id[2];
    bcm_gport_t egress_pw_encap_id[4];

    int ac_in_vlan;
    int ac_out_vlan;
    int pw_out_label[2];
    int lsp_out_lable[4];

    bcm_gport_t mpls_port_id[4];

    bcm_gport_t egress_pwe_port_id[8];
    bcm_gport_t egress_pwe_port_num;

    bcm_failover_t  pw_failover_id;

    bcm_failover_t primary_pw_lsp_failover_id;
    bcm_failover_t  protected_pw_lsp_failover_id;

    bcm_gport_t primary_pw_fec;
    bcm_gport_t protected_pw_fec;

    bcm_gport_t protection_fec_id;


    bcm_failover_t lsr_frr_failover_id;
    int lsr_in_label;
    int lsr_out_label;

    int primary_lsp_out_label;
    int backup_lsp_out_label;

    bcm_if_t primary_lsp_tunnel_id;
    bcm_if_t backup_lsp_tunnel_id;

    bcm_if_t lsr_frr_fec_id;

    bcm_if_t lsr_ecmp_id;


    int lsr_ecmp_label[4];
    int lsr_ecmp_tunnel_id[4];

    bcm_field_group_t group;
    bcm_field_entry_t ent;
};

mpls_info_t protection_mpls_info;



void mpls_info_init()
{
    protection_mpls_info.vpn_id = 6000;
    protection_mpls_info.access_port_in_vlan= 200;

    protection_mpls_info.ingress_mpls_port_id[0] = 0x4000;
    protection_mpls_info.ingress_mpls_port_id[1] = 0x4001;

    protection_mpls_info.egress_pw_encap_id[0] = 0x10000;
    protection_mpls_info.egress_pw_encap_id[1] = 0x10001;
    protection_mpls_info.egress_pw_encap_id[2] = 0x10002;
    protection_mpls_info.egress_pw_encap_id[3] = 0x10003;

    protection_mpls_info.ac_in_vlan = 10;
    protection_mpls_info.ac_out_vlan = 20;

    protection_mpls_info.pw_out_label[0] = 1999;
    protection_mpls_info.pw_out_label[1] = 2999;

    protection_mpls_info.lsp_out_lable[0] = 1000;
    protection_mpls_info.lsp_out_lable[1] = 2000;
    protection_mpls_info.lsp_out_lable[2] = 3000;
    protection_mpls_info.lsp_out_lable[3] = 4000;


    protection_mpls_info.lsr_in_label= 3000;
    protection_mpls_info.lsr_out_label = 4000;
    protection_mpls_info.primary_lsp_out_label= 5000;
    protection_mpls_info.backup_lsp_out_label= 6000;

    protection_mpls_info.lsr_ecmp_label[0]= 5000;
    protection_mpls_info.lsr_ecmp_label[1]= 6000;
    protection_mpls_info.lsr_ecmp_label[2]= 5000;
    protection_mpls_info.lsr_ecmp_label[3]= 6000;



    /*data entry info*/
    pro_data_entry_info.pw_fp = 0x33;
    pro_data_entry_info.pripw_outport = pw_port;
    pro_data_entry_info.backuppw_outport = pw_port;
    pro_data_entry_info.pripw_outslot = 0x0;
    pro_data_entry_info.backuppw_outslot = 0x1;
    pro_data_entry_info.pripw_flowid = 0x77;
    pro_data_entry_info.backuppw_flowid = 0x88;


    pro_data_entry_info.ac_fp = 0x77;
    pro_data_entry_info.ac_outport = ac_port;
    pro_data_entry_info.ac_outslot = 0x0;
    pro_data_entry_info.ac_flowid = 0x438;

    pro_data_entry_info.lsp_fp = 0x33;
    pro_data_entry_info.primarylsp_outport = 0;
    pro_data_entry_info.backuplsp_outport = 1;
    pro_data_entry_info.primarylsp_outslot = 0x0;
    pro_data_entry_info.backuplsp_outslot = 0x1;
    pro_data_entry_info.primarylsp_flowid = 0x77;
    pro_data_entry_info.backuplsp_flowid = 0x88;


}

int pp_port_to_system_port_on_np_connected_core(int unit, int pp_port, int *sysport) {
    int modid, gport, rv, tm_port, core;


    rv = bcm_stk_modid_get(unit, &modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_get\n");
        return rv;
    }
    if (is_device_or_above(unit,JERICHO)) {
        /* Core should also be considered */
        rv = get_core_and_tm_port_from_port(unit,ilk_port,&core,&tm_port);
        if (rv != BCM_E_NONE){
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }
    } else {
        core = 0;
        tm_port = pp_port;
    }
    BCM_GPORT_MODPORT_SET(gport, modid + core, pp_port);

    rv = bcm_stk_gport_sysport_get(unit, gport, sysport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_gport_sysport_get\n");
        return rv;
    }

    return BCM_E_NONE;
}

/* Init np port info */
int nph_service_info_egress_init(uint32 index, bcm_port_t np_port, uint32 ps, uint32 output_fp, bcm_gport_t gport)
{
    if (index >= nph_nof_services) {
        return BCM_E_PARAM;
    }

    if ((ps & (~nph_ps_mask)) != 0) {
        return BCM_E_PARAM;
    }
    
    if ((output_fp & (~nph_output_fp_mask)) != 0) {
        return BCM_E_PARAM;
    }

    g_nph_service_egress_info[index].np_port = np_port;
    g_nph_service_egress_info[index].ps = ps;
    g_nph_service_egress_info[index].output_fp = output_fp;
    g_nph_service_egress_info[index].gport = gport;

    return 0;
}


int nph_vpws_service_egress_run(int unit)
{
    int rv = 0;
    int index;
    bcm_port_t port;
    bcm_vlan_port_t vp;
    bcm_vswitch_cross_connect_t gports;

    for (index=0; index<nph_nof_services; index++) {
        bcm_vlan_port_t_init(&vp);    
        vp.port = g_nph_service_egress_info[index].np_port;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
        vp.match_tunnel_value = (g_nph_service_egress_info[index].ps << 17) | g_nph_service_egress_info[index].output_fp;
        rv = bcm_vlan_port_create(0, &vp);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_create failed $rv\n");
            return rv;
        }

        g_nph_service_egress_info[index].vlan_port_id = vp.vlan_port_id;

        printf("Created VLAN port for {ps, output_fp} (%d, 0x%x)\n", g_nph_service_egress_info[index].ps, g_nph_service_egress_info[index].output_fp);

        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        gports.port1 = g_nph_service_egress_info[index].vlan_port_id;
        gports.port2 = g_nph_service_egress_info[index].gport;

       /*for p2p destination service on egress chip, it should be egress only style*/
        if (BCM_GPORT_IS_MPLS_PORT(gports.port2)
             &&((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gports.port2) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))){
              gports.port2 = pw_port;
              gports.encap2 = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(g_nph_service_egress_info[index].gport);
        }
        /* cross connect the 2 ports */
        if(verbose >= 1) {
            printf("connect port1:0x0%8x with port2:0x0%8x \n", gports.port1, gports.port2);
        }
        
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            print rv;
            return rv;
        }
    }

    return rv;
}


/* initialize the tunnels for mpls routing */
int
protection_mpls_tunnels_config(
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

    label_array[0].exp = 0; /* Set it statically 0*/ 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].label = in_tunnel_label;
    label_array[0].ttl = 20;

    label_array[1].exp = 0; /* Set it statically 0 */ 
    label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[1].label = out_tunnel_label;
    label_array[1].ttl = 40;

    rv = bcm_mpls_tunnel_initiator_create(unit,0,2,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    print("tunnel id =0x%x",label_array[0].tunnel_id);
    create_l3_egress_s l3eg;

    sal_memcpy(l3eg.next_hop_mac_addr, cross_connect_info.nh_mac, 6);
    l3eg.vlan   = eg_vlan;
    l3eg.out_gport   = port;
    l3eg.out_tunnel_or_rif = label_array[0].tunnel_id;
    l3eg.allocation_flags = BCM_L3_EGRESS_ONLY;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", vlan);
        return rv;
    }


    /* create egress object points to this tunnel/interface */
    ingress_intf = label_array[0].tunnel_id;
    BCM_L3_ITF_SET(*egress_intf,BCM_L3_ITF_TYPE_LIF,label_array[0].tunnel_id);  
    return rv;
}


int protection_egress_pwe_create(int unit, bcm_mpls_port_t *mpls_port, int port, int out_vc_lable,int out_lsp_label, int eep)
{
    int rv = 0;
    uint64 data_udh;

    rv = protection_mpls_tunnels_config(unit, pw_port, &tunnel_id, out_lsp_label, out_lsp_label+100);
    if(rv != BCM_E_NONE) {
         printf("mpls_tunnels_config failed\n");
         return -1;
     }

  
    /* set port attribures */
    mpls_port->flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port->egress_tunnel_if = tunnel_id;    
    mpls_port->flags |= BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port->egress_label.label = out_vc_lable;    
    mpls_port->encap_id  = eep;        

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }

    protection_mpls_info.egress_pwe_port_id[protection_mpls_info.egress_pwe_port_num++] = mpls_port->mpls_port_id;

    printf("mpls.encap = 0x%x\n",mpls_port->encap_id);
    printf("pw created...\n");
    return rv;
}

int protection_config_cleanup(int unit, int vpls)
{
    int i = 0;
    int rv = 0;

    for(i = 0 ;i <4; i++ ){
          if (protection_mpls_info.mpls_port_id[i]) {
            bcm_mpls_port_delete(unit,vpls?protection_mpls_info.vpn_id:0,protection_mpls_info.mpls_port_id[i]);
            protection_mpls_info.mpls_port_id[i] = 0;
        }
    }

    for(i = 0;i<protection_mpls_info.egress_pwe_port_num;i++){
            bcm_mpls_port_delete(unit,0,protection_mpls_info.egress_pwe_port_id[i]);
            protection_mpls_info.egress_pwe_port_id[i] = 0;
    }

    bcm_vlan_port_destroy(unit, protection_mpls_info.access_port_id);

    /*mpls lsr configuration cleanup*/
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.label = protection_mpls_info.lsr_in_label;

    bcm_mpls_tunnel_switch_delete(unit, &entry);

    bcm_l3_egress_destroy(unit, protection_mpls_info.lsr_frr_fec_id);
    bcm_l3_egress_destroy(unit, protection_mpls_info.lsr_frr_fec_id+1);


    bcm_vlan_port_destroy(unit, pro_data_entry_info.primary_lsp_egress_ac);

    bcm_mpls_tunnel_initiator_clear(unit,protection_mpls_info.backup_lsp_tunnel_id);
    bcm_mpls_tunnel_initiator_clear(unit,protection_mpls_info.primary_lsp_tunnel_id);

    return 0;

}

int ingress_mpls_port_create_with_normal_protection(int unit)
{
     int rv;
     mpls_port__fec_only_info_s  mpls_port_fec_info;

    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &protection_mpls_info.pw_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE , rv - %d\n", rv);
        return rv;
    }
    printf(" PWE  Failover id: 0x%x\n",protection_mpls_info.pw_failover_id);

    data_entry_init(pro_data_entry_info.backuppw_outport,pro_data_entry_info.backuppw_flowid,pro_data_entry_info.backuppw_outslot,pro_data_entry_info.pw_fp,0/*PS:0b000*/,0,0);
    data_format_entry_create(unit,&pro_data_entry_info.protected0_pw_eedb_index);

    mpls_port_fec_info.port_id = ilk_port;
    mpls_port_fec_info.failover_id = protection_mpls_info.pw_failover_id;
    mpls_port_fec_info.failover_port_id = 0;/* this is secondary*/
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, pro_data_entry_info.protected0_pw_eedb_index);


   /*create the primay pw*/
    rv =  mpls_port__fec_only__create(unit,&mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create for protected PW , rv - %d\n", rv);
        return rv;
    }
    printf("protectedPW portid : 0x%x\n",mpls_port_fec_info.gport);


    data_entry_init(pro_data_entry_info.pripw_outport,pro_data_entry_info.pripw_flowid,pro_data_entry_info.pripw_outslot,pro_data_entry_info.pw_fp,1/*PS:0b001*/,0,0);
    data_format_entry_create(unit,&pro_data_entry_info.primary0_pw_eedb_index);

    mpls_port_fec_info.port_id = ilk_port;
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, pro_data_entry_info.primary0_pw_eedb_index);
     mpls_port_fec_info.failover_port_id = mpls_port_fec_info.gport;
     mpls_port_fec_info.gport = 0;

     mpls_port__fec_only__create(unit,&mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create for primary PW, rv - %d\n", rv);
        return rv;
    }
    printf("primary PW  portid : 0x%x\n", mpls_port_fec_info.gport);

    protection_mpls_info.protection_fec_id =  mpls_port_fec_info.gport;

}


int ingress_mpls_port_create_with_hierarchical_protection(int unit)
{
     int rv;
     mpls_port__fec_only_info_s  mpls_port_fec_info;
    
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &protection_mpls_info.primary_pw_lsp_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for primary PWE LSP, rv - %d\n", rv);
        return rv;
    }
    printf("primary PWE lsp Failover id: 0x%x\n", protection_mpls_info.primary_pw_lsp_failover_id);


    data_entry_init(pro_data_entry_info.pripw_outport,pro_data_entry_info.pripw_flowid,pro_data_entry_info.pripw_outslot,pro_data_entry_info.pw_fp,1/*PS:0b001*/,0,0);
    data_format_entry_create(unit,&pro_data_entry_info.primary1_pw_eedb_index);

    mpls_port_fec_info.port_id = ilk_port;
    mpls_port_fec_info.failover_id = protection_mpls_info.primary_pw_lsp_failover_id;
    mpls_port_fec_info.failover_port_id = 0;/* this is secondary*/
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, pro_data_entry_info.primary1_pw_eedb_index);


    /*create the primay pw*/
    rv =  mpls_port__fec_only__create(unit,&mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create for primary PW on primary lsp, rv - %d\n", rv);
        return rv;
    }
    printf("primary PW on primay lsp portid : 0x%x\n", mpls_port_fec_info.gport);

    data_entry_init(pro_data_entry_info.pripw_outport,pro_data_entry_info.pripw_flowid,pro_data_entry_info.pripw_outslot,pro_data_entry_info.pw_fp,3/*PS:0b011*/,0,0);
    data_format_entry_create(unit,&pro_data_entry_info.primary0_pw_eedb_index);

    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, pro_data_entry_info.primary0_pw_eedb_index);
    mpls_port_fec_info.failover_port_id = mpls_port_fec_info.gport;
    mpls_port_fec_info.gport = 0;

     mpls_port__fec_only__create(unit,&mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create for primary PW on protected lsp, rv - %d\n", rv);
        return rv;
    }
    printf("primary PW on protected lsp portid : 0x%x\n", mpls_port_fec_info.gport);

    BCM_L3_ITF_SET(protection_mpls_info.primary_pw_fec, BCM_L3_ITF_TYPE_FEC, BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_fec_info.gport));


    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &protection_mpls_info.protected_pw_lsp_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for protected PWE LSP, rv - %d\n", rv);
       return rv;
    }
    printf("protected PWE lsp Failover id: 0x%x\n", protection_mpls_info.protected_pw_lsp_failover_id);

    data_entry_init(pro_data_entry_info.backuppw_outport,pro_data_entry_info.backuppw_flowid,pro_data_entry_info.backuppw_outslot,pro_data_entry_info.pw_fp,0/*PS:0b000*/,0,0);
    data_format_entry_create(unit,&pro_data_entry_info.protected1_pw_eedb_index);

    mpls_port_fec_info.port_id = ilk_port;
    mpls_port_fec_info.failover_id = protection_mpls_info.protected_pw_lsp_failover_id;
    mpls_port_fec_info.failover_port_id = 0;/* this is secondary*/
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, pro_data_entry_info.protected1_pw_eedb_index);
    mpls_port_fec_info.gport = 0;

    /*create the backup pw*/
    rv =  mpls_port__fec_only__create(unit,&mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create for primary PW on primary lsp, rv - %d\n", rv);
        return rv;
    }
    printf("protected PW on primay lsp portid : 0x%x\n", mpls_port_fec_info.gport);

    data_entry_init(pro_data_entry_info.backuppw_outport,pro_data_entry_info.backuppw_flowid,pro_data_entry_info.backuppw_outslot,pro_data_entry_info.pw_fp,2/*PS:0b010*/,0,0);
    data_format_entry_create(unit,&pro_data_entry_info.protected0_pw_eedb_index);

    mpls_port_fec_info.failover_port_id = mpls_port_fec_info.gport;/* this is secondary*/
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, pro_data_entry_info.protected0_pw_eedb_index);
    mpls_port_fec_info.gport = 0;

     mpls_port__fec_only__create(unit,&mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create for protected PW on protected lsp, rv - %d\n", rv);
        return rv;
    }
    printf("protected PW on protected lsp portid : 0x%x\n", mpls_port_fec_info.gport);

    BCM_L3_ITF_SET(protection_mpls_info.protected_pw_fec, BCM_L3_ITF_TYPE_FEC, BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_fec_info.gport));



    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &protection_mpls_info.pw_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for  PW , rv - %d\n", rv);
        return rv;
    }
    printf(" PW Failover id: 0x%x\n", protection_mpls_info.pw_failover_id);

    bcm_if_t fec_id1;
    bcm_if_t fec_id2;

    int rc = 0;
    bcm_l3_egress_t l3eg_fec_1;
    bcm_l3_egress_t_init(&l3eg_fec_1);
    /*l3eg_fec_1.intf = tunnel_id_1;     */                                        /* Choose level-1 MPLS tunnel */
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(l3eg_fec_1.port, protection_mpls_info.protected_pw_fec); /* level-2 MPLS tunnel will be choosed by ECMP */
    l3eg_fec_1.failover_id = protection_mpls_info.pw_failover_id;                                  /* Protect FEC to enable hierarchical FEC */
    l3eg_fec_1.failover_if_id = 0;

    l3eg_fec_1.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    rc = bcm_l3_egress_create(unit,BCM_L3_INGRESS_ONLY,&l3eg_fec_1,&fec_id1);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rc;
    }
    printf("FEC created. ID = 0x%x\n", fec_id1);

    bcm_l3_egress_t l3eg_fec_2;
    bcm_l3_egress_t_init(&l3eg_fec_2);
    /*l3eg_fec_2.intf = tunnel_id_2;*/
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(l3eg_fec_2.port,protection_mpls_info.primary_pw_fec);
    l3eg_fec_2.failover_id = protection_mpls_info.pw_failover_id;
    l3eg_fec_2.failover_if_id = fec_id1;

    l3eg_fec_2.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;

    rc = bcm_l3_egress_create(unit,BCM_L3_INGRESS_ONLY,&l3eg_fec_2,&fec_id2);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rc;
    }
    printf("FEC created. ID = 0x%x,\n", fec_id2);


    /*BCM_GPORT_FORWARD_PORT_SET(hprotection_fec_id,BCM_L3_ITF_VAL_GET(fec_id2));*/
     BCM_GPORT_MPLS_PORT_ID_SET(protection_mpls_info.protection_fec_id,BCM_L3_ITF_VAL_GET(fec_id2));
}



int protection_vpws_ac2pw_ingress_config(int unit, int protection_mode)
{
       int rv;
        bcm_vswitch_cross_connect_t vpws_info;
      

      rv = ac_create(unit, &vlan_port, ac_port, 10);
     if (rv != BCM_E_NONE) {
        printf("Error, ac_create: $rv\n");
        return rv;
    }

       /*config vlan status for ac port*/
      /* config_ivec_value_mapping(unit,1);*/

    /* config_vlan_status_on_lif(unit,vlan_port.vlan_port_id,1);*/


       if (1 == protection_mode) {
          ingress_mpls_port_create_with_hierarchical_protection(unit);
       } else {
          ingress_mpls_port_create_with_normal_protection(unit);
        }

        bcm_vswitch_cross_connect_t_init(&vpws_info);
        vpws_info.flags |= BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        vpws_info.port1 = vlan_port.vlan_port_id;
        vpws_info.port2 = protection_mpls_info.protection_fec_id;

        protection_mpls_info.access_port_id = vlan_port.vlan_port_id;

        printf("connect port1:0x0%8x with port2:0x0%8x \n", vpws_info.port1, vpws_info.port2);

        rv = bcm_vswitch_cross_connect_add(unit, &vpws_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add,rv = 0x%x\n",rv);
            return rv;
        }

}
 
int protection_vpws_ac2pw_egress_config(int unit, int protection_mode)
{
       int rv = 0;
       int ps = 0;  /*0b000: protected pw, 
                             0b001: primary pw
                             0b010: protected pw with primay lsp
                             0b011: primay pw with primay lsp*/
       int eedbnum = 0;
     
     /*Just for initiation of pw data, not really for p2p*/
       eedbnum= protection_mode?4:2;
       nph_nof_services = eedbnum;
     cross_connect_info_init(ac_port, 0, pw_port, 0);

    for (ps = 0; ps<eedbnum;ps++) {
           bcm_mpls_port_t_init(mpls_port);
          
          rv = protection_egress_pwe_create(unit, &mpls_port, pw_port,protection_mpls_info.pw_out_label[ps%2],protection_mpls_info.lsp_out_lable[ps],protection_mpls_info.egress_pw_encap_id[ps]);
          if (rv != BCM_E_NONE) {
            printf("Error, ac_create: $rv\n");
            return rv;
            }
                  nph_service_info_egress_init(ps, ilk_port, ps, pro_data_entry_info.pw_fp, protection_mpls_info.egress_pwe_port_id[ps]);
            }
        nph_vpws_service_egress_run(unit);

}

int protection_vpws_pw2ac_ingress_config(int unit, int protection_mode)
{
       int rv;
        bcm_vswitch_cross_connect_t vpws_info;
      int eedbnum = 0;
      
      int ps = 0;
     bcm_gport_t  il_gport_id = 0;

      data_entry_init(pro_data_entry_info.ac_outport,pro_data_entry_info.ac_flowid,pro_data_entry_info.ac_outslot,pro_data_entry_info.ac_fp,1/*PS:0b001*/,0,0);
      data_format_entry_create(unit,&pro_data_entry_info.ac_eedb_index);

      BCM_GPORT_LOCAL_SET(il_gport_id, ilk_port);


      bcm_vswitch_cross_connect_t_init(&vpws_info);
       vpws_info.flags |= BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

       vpws_info.port2 =      il_gport_id;
       vpws_info.encap2 = pro_data_entry_info.ac_eedb_index;

       for (ps = 0;ps<eedbnum;ps++) {
          bcm_mpls_port_t_init(&mpls_port);
          mpls_port.match_label = protection_mpls_info.pw_out_label[ps]+1000; /*in vc label*/
         BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id,protection_mpls_info.ingress_mpls_port_id[ps]);
          mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
          mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_WITH_ID;
          mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_WIDE | BCM_MPLS_PORT2_INGRESS_ONLY;
          mpls_port.failover_port_id = protection_mpls_info.protection_fec_id;
           
          rv = bcm_mpls_port_add(unit, protection_mpls_info.vpn_id,&mpls_port);
          if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add: $rv\n");
            return rv;
          }

             vpws_info.port1 = mpls_port.mpls_port_id;


             printf("connect port1:0x0%8x with port2:0x0%8x \n", vpws_info.port1, vpws_info.port2);

            rv = bcm_vswitch_cross_connect_add(unit, &vpws_info);
            if (rv != BCM_E_NONE) {
               printf("Error, bcm_vswitch_cross_connect_add,rv = 0x%x\n",rv);
              return rv;
             }
       }
 

}

int protection_vpws_pw2ac_egress_config(int unit, int protection_mode)
{
        nph_nof_services = 1;
        nph_service_info_egress_init(0, ilk_port, 1, pro_data_entry_info.ac_fp,  vlan_port.vlan_port_id);   
        nph_vpws_service_egress_run(unit);
}

int protection_vpls_network_port_create(int unit, int h_protection)
{
     int rv=0;
     int port_num  = 0,ps = 0;


    /*create the protection group, returned FEC as learn data*/
    if (h_protection) {
        ingress_mpls_port_create_with_hierarchical_protection(unit);
    } else {
        ingress_mpls_port_create_with_normal_protection(unit);
   }

       port_num= h_protection?4:2;

    /*create the mpls port*/
        for (ps = 0; ps<port_num;ps++) {
          bcm_mpls_port_t_init(&mpls_port);
          rv = protection_egress_pwe_create(unit, &mpls_port, pw_port,protection_mpls_info.pw_out_label[ps%2],protection_mpls_info.lsp_out_lable[ps],protection_mpls_info.egress_pw_encap_id[ps]);
          if (rv != BCM_E_NONE) {
            printf("Error, ac_create: $rv\n");
            return rv;
          }
             protection_mpls_info.mpls_port_id[ps] = mpls_port.mpls_port_id;
              nph_service_info_egress_init(ps, ilk_port, ps, pro_data_entry_info.pw_fp, protection_mpls_info.egress_pwe_port_id[ps]);

        }
       /*map FP+PS  to mpls port*/
       nph_vpws_service_egress_run(unit);

      for(ps=0; ps < 2 ;ps ++){
          bcm_mpls_port_t_init(&mpls_port);
          mpls_port.match_label = protection_mpls_info.pw_out_label[ps]+1000; /*in vc label*/
         BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id,protection_mpls_info.ingress_mpls_port_id[ps]);
          mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
          mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_WITH_ID;
          mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_WIDE | BCM_MPLS_PORT2_INGRESS_ONLY;
          mpls_port.failover_port_id = protection_mpls_info.protection_fec_id;

          rv = bcm_mpls_port_add(unit, protection_mpls_info.vpn_id,&mpls_port);
          if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add: $rv\n");
            return rv;
          }
          protection_mpls_info.mpls_port_id[ps] = mpls_port.mpls_port_id;

      }
      
     return rv;
}

int protection_vpls_access_port_create(int unit)
{
    int rv;  
    bcm_vlan_port_t vlan_port;
    
    
    /* add port, according to port_vlan_vlan */
    bcm_vlan_port_t_init(&vlan_port);

    
    /* set port attribures, key <port-vlan-vlan>*/
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port;
    vlan_port.match_vlan = protection_mpls_info.access_port_in_vlan;
    vlan_port.egress_vlan = protection_mpls_info.access_port_in_vlan;
    vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    protection_mpls_info.access_port_id = vlan_port.vlan_port_id;

    printf("ac..encap = 0x%x\n",vlan_port.encap_id);
    printf("ac created....\n");
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }

    rv = bcm_vswitch_port_add(unit, protection_mpls_info.vpn_id, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

   /*Here we don't add the access port to bc group of vsi ,it's just for demo uc protection*/    
    return rv;

}



int protection_vpls_init(int unit)
{
     int rv = 0;

    mpls_info_init();

    /*Here we just use these data structure ,not really configure P2P service*/
    cross_connect_info_init(ac_port, 0, pw_port, 0);
    config_ilk_port(unit,ilk_port);

     mpls_info_init();


    rv = mpls_port__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__init\n");
        return rv;
    }

    if (!is_device_or_above(unit,JERICHO)) {
        printf("Error, Hierarchical VPLS is supported only from Jericho \n");
        return -1;
    }

    port_tpid_init(pw_port,1,0);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }
    port_tpid_init(ac_port,1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
        are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }
   
    /* Init vlan */  
    vlan__init_vlan(unit,protection_mpls_info.ac_in_vlan);

    if (verbose1) {
        printf("vswitch_vpls_hvpls_run create vswitch\n");
    }
    /* create vswitch */
    rv = mpls_port__vswitch_vpls_vpn_create__set(unit, protection_mpls_info.vpn_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_vpn_create\n");
        return rv;
    }

     return rv;
}

int protection_mpls_lsr_frr_group_create(int unit)
{
    int rv=0;
    create_l3_egress_s l3_egress_fec;

    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &protection_mpls_info.lsr_frr_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE , rv - %d\n", rv);
        return rv;
    }


    data_entry_init(pro_data_entry_info.backuplsp_outport,pro_data_entry_info.backuplsp_flowid,pro_data_entry_info.backuplsp_outslot,pro_data_entry_info.lsp_fp,0/*PS:0b000*/,0,3);
    data_format_entry_create(unit,&pro_data_entry_info.backuplsp_eedb_index);

    data_entry_init(pro_data_entry_info.primarylsp_outport,pro_data_entry_info.primarylsp_flowid,pro_data_entry_info.primarylsp_outslot,pro_data_entry_info.lsp_fp,1/*PS:0b000*/,0,3);
    data_format_entry_create(unit,&pro_data_entry_info.primarylsp_eedb_index);

    /* create FEC */
    BCM_L3_ITF_SET(l3_egress_fec.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,pro_data_entry_info.backuplsp_eedb_index);  
    l3_egress_fec.out_gport   = ilk_port;
    l3_egress_fec.failover_id = protection_mpls_info.lsr_frr_failover_id;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }

    BCM_L3_ITF_SET(l3_egress_fec.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,pro_data_entry_info.primarylsp_eedb_index);  
    l3_egress_fec.failover_id = protection_mpls_info.lsr_frr_failover_id;
    l3_egress_fec.failover_if_id = l3_egress_fec.fec_id;
    l3_egress_fec.fec_id = 0;
    rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create\n");
        return rv;
    }

    protection_mpls_info.lsr_frr_fec_id = l3_egress_fec.fec_id;

}

int protection_mpls_lsr_frr_ingress_config(int unit, int is_ecmp)
{
    int rv = 0;
    bcm_pbmp_t pbmp,ubmp;
    bcm_vlan_t vlan;

 
    /* open vlan */
    vlan = cross_connect_info.vlan_base++;
    printf("open vlan %d\n", vlan);
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", vlan);
        print rv;
    }

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, ac_port);

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
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
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
    
    protection_mpls_lsr_frr_group_create(unit);

    bcm_if_t out_fec;
    out_fec = is_ecmp?protection_mpls_info.lsr_ecmp_id:protection_mpls_info.lsr_frr_fec_id;
    rv = mpls_add_switch_entry(unit,protection_mpls_info.lsr_in_label, protection_mpls_info.lsr_out_label, out_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_switch_entry\n");
        return rv;
    }


    return rv;
}

int nph_egr_pmf_grp_info_init(int unit ,int pp_port){
     bcm_field_qset_t  qset;
     bcm_field_aset_t  aset;
     bcm_field_group_t group;
    int pri=10;
    uint64 vsdata,vsmask;
    int rv;
    int tag_format,vlan_status;

 
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort); 
    rv = bcm_field_group_create(0, qset, pri, &group);
    if (rv != BCM_E_NONE){
        printf("bcm_field_group_create failed %d\n",rv);
    }    

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionForwardingTypeNew);

    rv = bcm_field_group_action_set(0, group, aset);
    if (rv != BCM_E_NONE){
        printf("bcm_field_group_action_set failed %d\n",rv);
    }    

    rv = bcm_field_group_install(0, group);
    if (rv != BCM_E_NONE){
        printf("bcm_field_group_install failed %d\n",rv);
    }    


    /*install the entry*/

    bcm_field_entry_t ent;
    bcm_gport_t in_gport;
    bcm_field_action_core_config_t config;

    bcm_field_entry_create(0, group, &ent);         
    if (rv != BCM_E_NONE){
        printf("bcm_field_entry_create failed %d\n",rv);
    }    

    /*BCM_GPORT_LOCAL_SET(in_gport, pp_port);*/
    rv = bcm_field_qualify_InPort(unit, ent, pp_port, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_SrcPort returned %d (%s)\n", "nph_egr_pmf_grp_info_init", rv, bcm_errmsg(rv));
        return rv;
    }

    config.param0 = bcmFieldForwardingTypeMpls;
    config.param1 = 1; /* forwarding header index */
    config.param2 = 0; /* forwarding offset in the header */
    bcm_field_action_config_add(0, ent, bcmFieldActionForwardingTypeNew, 1,&config);        
    rv = bcm_field_entry_install(0, ent);
    if (rv != BCM_E_NONE){
        printf("bcm_field_entry_install failed %d\n",rv);
    }    

    protection_mpls_info.group = group;
    protection_mpls_info.ent = ent;

}

int nph_egr_pmf_grp_info_deinit(int unit ){

    bcm_field_entry_destroy(unit, protection_mpls_info.ent);
    bcm_field_group_destroy(unit, protection_mpls_info.group);
}


int protection_mpls_lsr_frr_egress_config(int unit)
{
    int rv=0;
    int index=0,del_len=0;
    bcm_pbmp_t pbmp,ubmp;
    bcm_vlan_t vlan;
    bcm_vlan_port_t vlan_port;
    bcm_vswitch_cross_connect_t gports;
    bcm_port_extender_mapping_info_t mapping_info;
    bcm_gport_t sys_port;
    int pp_port = 32;

    nph_egr_pmf_grp_info_init(unit,pp_port);

    mapping_info.user_define_value=(nph_uc<<10)|(nph_sub_type_swap << 6) |18/*del_len 18*/;
    mapping_info.pp_port=pp_port;
    mapping_info.phy_port=ilk_port;
    /* Map the source board and souce port to in pp port */
    rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }
    rv = pp_port_to_system_port_on_np_connected_core(unit, pp_port, &sys_port);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = bcm_port_class_set(unit, sys_port, bcmPortClassId, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_class_set failed $rv\n");
        return rv;
    }


    /*create the mid port for mpls lsr*/
    bcm_vlan_port_t_init(&vlan_port);    
    index=1;
    vlan_port.port = sys_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (0x00 << 17) | pro_data_entry_info.lsp_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }
    printf("backup lsr ac lif 0x%x",vlan_port.vlan_port_id);
    pro_data_entry_info.backup_lsp_egress_ac = vlan_port.vlan_port_id;

    /*create the mid port for mpls lsr*/
    bcm_vlan_port_t_init(&vlan_port);    
    index=0;
    vlan_port.port = sys_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (0x01 << 17) | pro_data_entry_info.lsp_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }
    printf("primary lsr ac lif 0x%x",vlan_port.vlan_port_id);
    pro_data_entry_info.primary_lsp_egress_ac = vlan_port.vlan_port_id;

    int out_rif_vlan;
    /* open vlan */
    out_rif_vlan = cross_connect_info.vlan_base++;
    printf("open vlan %d\n", out_rif_vlan);
    rv = bcm_vlan_create(unit, out_rif_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", out_rif_vlan);
        print rv;
    }

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, pw_port);

    rv = bcm_vlan_port_add(unit, out_rif_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", out_rif_vlan);
        print rv;
        return rv;
    }
 
    /* create the out rif for mpls lsr egress  */
    create_l3_intf_s intf;
    intf.vsi = out_rif_vlan;
    intf.my_global_mac = cross_connect_info.my_mac;
    intf.my_lsb_mac = cross_connect_info.my_mac;
    intf.skip_mymac = 1;
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }

    /*create backup lsp tunnel and connect ac port to it*/
    bcm_mpls_egress_label_t label_array[2];

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].exp = 0; /* Set it statically 0*/ 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    label_array[0].flags =BCM_MPLS_EGRESS_LABEL_ACTION_VALID;  
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;

    label_array[0].label = protection_mpls_info.backup_lsp_out_label;
    label_array[0].ttl = 20;

    rv = bcm_mpls_tunnel_initiator_create(unit,0,1,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    protection_mpls_info.backup_lsp_tunnel_id = label_array[0].tunnel_id;

    create_l3_egress_s l3eg;

    sal_memcpy(l3eg.next_hop_mac_addr, cross_connect_info.nh_mac, 6);
    l3eg.vlan   = out_rif_vlan;
    l3eg.out_gport   = pw_port;
    l3eg.out_tunnel_or_rif = label_array[0].tunnel_id;
    l3eg.allocation_flags = BCM_L3_EGRESS_ONLY;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress__create rv=%d \n", rv);
        return rv;
    }
    printf("backup egress object id =0x%x\n",l3eg.arp_encap_id);

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = pro_data_entry_info.backup_lsp_egress_ac;
    gports.port2 = pw_port;
    gports.encap2 = BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id);
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }


    /*create primary lsp tunnel and connect ac port to it*/
    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].exp = 0; /* Set it statically 0*/ 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    label_array[0].flags =BCM_MPLS_EGRESS_LABEL_ACTION_VALID;  
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;
    label_array[0].label = protection_mpls_info.primary_lsp_out_label;
    label_array[0].ttl = 20;

    rv = bcm_mpls_tunnel_initiator_create(unit,0,1,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    protection_mpls_info.primary_lsp_tunnel_id = label_array[0].tunnel_id;
    create_l3_egress_s l3eg1;

    sal_memcpy(l3eg1.next_hop_mac_addr, cross_connect_info.nh_mac, 6);
    l3eg1.vlan   = out_rif_vlan;
    l3eg1.out_gport   = pw_port;
    l3eg1.out_tunnel_or_rif = label_array[0].tunnel_id;
    l3eg1.allocation_flags = BCM_L3_EGRESS_ONLY;

    rv = l3__egress__create(unit,&l3eg1);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress__create rv=%d \n", rv);
        return rv;
    }
    printf("primary egress object id =0x%x\n",l3eg.arp_encap_id);


    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = pro_data_entry_info.primary_lsp_egress_ac;
    gports.port2 = pw_port;
    gports.encap2 = BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id);
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    return nph_mpls_egr_fwd_code_and_offset_set(0);
}

int protection_mpls_lsr_ecmp_group_create(int unit)
{
    int rv=0;
    create_l3_egress_s l3_egress_fec;
    int i = 0;


    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &protection_mpls_info.lsr_frr_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE , rv - %d\n", rv);
        return rv;
    }

    for (i = 0; i< 4 ;i++){
        data_entry_init(pro_data_entry_info.backuplsp_outport,pro_data_entry_info.backuplsp_flowid,pro_data_entry_info.backuplsp_outslot,pro_data_entry_info.lsp_fp+i,1/*PS:0b001*/,1,3);
        data_format_entry_create(unit,&pro_data_entry_info.lsr_ecmp_eedb_index[i]);
    }

    for (i = 0; i< 4 ;i++){
        /* create FEC */
        BCM_L3_ITF_SET(l3_egress_fec.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,pro_data_entry_info.lsr_ecmp_eedb_index[i]);  
        l3_egress_fec.out_gport   = ilk_port;
        rv = l3__egress_only_fec__create(unit, &l3_egress_fec);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_fec__create\n");
            return rv;
        }
        pro_data_entry_info.lsr_ecmp_fec_index[i] = l3_egress_fec.fec_id;
    }

    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.max_paths = 4;

    rv = bcm_l3_egress_ecmp_create(unit, &ecmp, 4, pro_data_entry_info.lsr_ecmp_fec_index);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rv;
    }

    protection_mpls_info.lsr_ecmp_id = ecmp.ecmp_intf;

}

int protection_mpls_lsr_ecmp_ingress_config(int unit)
{
    protection_mpls_lsr_ecmp_group_create(0);
    protection_mpls_lsr_frr_ingress_config(unit,1);
}

int protection_mpls_lsr_ecmp_egress_config(int unit)
{
    int rv=0;
    int index=0,del_len=0;
    bcm_pbmp_t pbmp,ubmp;
    bcm_vlan_t vlan;
    bcm_vlan_port_t vlan_port;
    bcm_vswitch_cross_connect_t gports;

    bcm_gport_t sys_port;
/*
    int pp_port = 32;

    rv = pp_port_to_system_port_on_np_connected_core(unit, pp_port, &sys_port);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port for out_port\n");
        return rv;
    }

    rv = bcm_port_class_set(unit, sys_port, bcmPortClassId, ilk_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_class_set failed $rv\n");
        return rv;
    }

*/
    int out_rif_vlan;
    /* open vlan */
    out_rif_vlan = cross_connect_info.vlan_base++;
    printf("open vlan %d\n", out_rif_vlan);
    rv = bcm_vlan_create(unit, out_rif_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", out_rif_vlan);
        print rv;
    }

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, pw_port);

    rv = bcm_vlan_port_add(unit, out_rif_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", out_rif_vlan);
        print rv;
        return rv;
    }
 
    /* create the out rif for mpls lsr egress  */
    create_l3_intf_s intf;
    intf.vsi = out_rif_vlan;
    intf.my_global_mac = cross_connect_info.my_mac;
    intf.my_lsb_mac = cross_connect_info.my_mac;
    intf.skip_mymac = 1;
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }

    /*create the mid port for mpls lsr*/
    int i = 0;
    bcm_mpls_egress_label_t label_array[2];

    for(i=0 ;i < 4 ; i++) {
        bcm_vlan_port_t_init(&vlan_port);    
        vlan_port.port = sys_port;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
        vlan_port.match_tunnel_value = (0x00 << 17) | pro_data_entry_info.lsp_fp+i;
        rv = bcm_vlan_port_create(0, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_create failed $rv\n");
            return rv;
        }
        printf("backup lsr ac lif 0x%x",vlan_port.vlan_port_id);
        pro_data_entry_info.lsp_ecmp_egress_ac[i]= vlan_port.vlan_port_id;

        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);

        label_array[0].exp = 0; /* Set it statically 0*/ 
        label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
        label_array[0].flags =BCM_MPLS_EGRESS_LABEL_ACTION_VALID;  
        label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;

        label_array[0].label = protection_mpls_info.backup_lsp_out_label;
        label_array[0].ttl = 20;

        rv = bcm_mpls_tunnel_initiator_create(unit,0,1,label_array);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_mpls_tunnel_initiator_create\n");
            return rv;
        }
        protection_mpls_info.lsr_ecmp_tunnel_id[i] = label_array[0].tunnel_id;

        create_l3_egress_s l3eg;

        sal_memcpy(l3eg.next_hop_mac_addr, cross_connect_info.nh_mac, 6);
        l3eg.vlan   = out_rif_vlan;
        l3eg.out_gport   = pw_port;
        l3eg.out_tunnel_or_rif = label_array[0].tunnel_id;
        l3eg.allocation_flags = BCM_L3_EGRESS_ONLY;

        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress__create rv=%d \n", rv);
            return rv;
        }
        printf("backup egress object id =0x%x\n",l3eg.arp_encap_id);

        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        gports.port1 = pro_data_entry_info.lsp_ecmp_egress_ac[i];
        gports.port2 = pw_port;
        gports.encap2 = BCM_L3_ITF_VAL_GET(label_array[0].tunnel_id);
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("bcm_vswitch_cross_connect_add failed $rv\n");
            return rv;
        }

    }


    return rv; 

}

int protection_vpws_config_with_default(int unit, int mode, int dvapi)
{

    int rv = 0;


    space_optimize_enable = 1;   
    cross_connect_info_init(ac_port, 0, pw_port, 0);

    mpls_info_init();

    config_ilk_port(unit,ilk_port);
   
     protection_vpws_ac2pw_ingress_config(unit,mode);
     protection_vpws_ac2pw_egress_config(unit,mode);

     if (!dvapi) {
         protection_vpws_pw2ac_ingress_config(unit,mode);
         protection_vpws_pw2ac_egress_config(unit,mode);
    }

    return rv;

}

int protection_vpws_config_with_default_dvapi(int unit, int mode)
{

    int rv = 0;

    bcm_port_loopback_set(unit,ilk_port,1);

   rv = protection_vpws_config_with_default(unit,mode,1);
   return rv;

}


int protection_vpls_config_with_default(int unit, int mode)
{

    int rv = 0;

    space_optimize_enable = 1;   
    /*Enlarge the table if is H-protection mode*/
    nph_nof_services = mode?4:2;
    rv = protection_vpls_init(unit);
    if (rv != BCM_E_NONE){
         printf("protection_vpls_init Error \n");
         return rv;
    }
  
    rv = protection_vpls_network_port_create(unit,mode);
    if (rv != BCM_E_NONE){
         printf("protection_vpls_network_port_create Error \n");
         return rv;
    }

    rv = protection_vpls_access_port_create(unit);
    if (rv != BCM_E_NONE){
         printf("protection_vpls_access_port_create Error \n");
         return rv;
    }

   return rv;
}

int protection_vpls_config_with_default_dvapi(int unit, int mode)
{
    int rv = 0;

    bcm_port_loopback_set(unit,ilk_port,1);

   rv = protection_vpls_config_with_default(unit,mode);
   return rv;
}

int protection_pw_failover_set(int unit, int state)
{
       bcm_failover_set(unit,protection_mpls_info.pw_failover_id,state);
       return 0;
}

int protection_lsp_failover_set(int unit,int pri_pw_lsp,int state)
{
      if (1 ==pri_pw_lsp ) {
       bcm_failover_set(unit,protection_mpls_info.primary_pw_lsp_failover_id,state);
     } else {
       bcm_failover_set(unit,protection_mpls_info.protected_pw_lsp_failover_id,state);
     }
     return 0;
}

int protection_mpls_lsr_frr_config_with_default(int unit, int dvapi)
{

    int rv = 0;

    if (dvapi) {
        bcm_port_loopback_set(unit,ilk_port,1);
    }

    cross_connect_info_init(ac_port, 0, pw_port, 0);

    mpls_info_init();

    config_ilk_port(unit,ilk_port);
   
    protection_mpls_lsr_frr_ingress_config(unit,0);

    protection_mpls_lsr_frr_egress_config(unit);
    return rv;

}

int protection_mpls_frr_failover_set(int unit, int state)
{
    bcm_failover_set(unit,protection_mpls_info.lsr_frr_failover_id,state);
    return 0;
}

int protection_mpls_lsr_ecmp_config_with_default(int unit, int dvapi)
{

    int rv = 0;

    if (dvapi) {
        bcm_port_loopback_set(unit,ilk_port,1);
    }

    cross_connect_info_init(ac_port, 0, pw_port, 0);

    mpls_info_init();

    config_ilk_port(unit,ilk_port);

    if ((rv=nph_pp_port_mapping_init(unit, 0, ilk_port, 128, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }


   
    protection_mpls_lsr_ecmp_ingress_config(unit);

    protection_mpls_lsr_ecmp_egress_config(unit);
    return rv;

}


