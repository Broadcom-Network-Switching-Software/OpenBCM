/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
* File: cint_pon_up_ratelimit.c  
* 
* Purpose: examples of two level ratelmits based on upstream.
*          First level ratelimit is done per outer VLAN within LLID.
*          Second level ratelimit is done per LLID.
* 
* up ratelimit Test cases:
*     1. Limit traffic of Tunnel 100, Outer VLAN 10 to 20000kbps.
*     2. Limit traffic of Tunnel 100, Outer VLAN 20 to 40000kbps.
*     3. Limit traffic of Tunnel 100, Outer VLAN 30 to 60000kbps.
*     4. Limit traffic of Tunnel 100, Outer VLAN 40 to 100000kbps.
*     5. Limit traffic of Tunnel 100, Outer VLAN 50 to 200000kbps.
*     The whole traffic of Tunnel 100 is limited to 200000kbps.
*
* PP Model: N:1 Service.
*     PON Port 4               <-----------------> NNI Port 128 
*     Tunnel-ID 100 CVLAN 10   <--------|
*     Tunnel-ID 100 CVLAN 20   <--------|
*     Tunnel-ID 100 CVLAN 30   <--------|--------> CVLAN 100
*     Tunnel-ID 100 CVLAN 40   <--------|
*     Tunnel-ID 100 CVLAN 50   <--------|
*
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/cint_port_tpid.c
*     BCM> cint examples/dpp/pon/cint_pon_utils.c
*     BCM> cint examples/dpp/pon/cint_pon_up_ratelimit.c
*     BCM> cint
* To Activate PON service models:
*     cint> pon_up_ratelimit_service_init(0, 4, 128);
*     cint> pon_up_ratelimit_service_setup(0);
* To Activate ACL Classification Run:
*     cint> pon_up_ratelimit_setup(0);
* To Deactivate ACL Classification Run:
*     cint> pon_up_ratelimit_cleanup(0);
* To Dectivate PON service models:
*     cint> pon_up_ratelimit_service_cleanup(0);
 */

struct pon_up_ratelimit_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    int down_lif_type;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    bcm_field_entry_t entry;
};

struct pon_llid_up_ratelimit_s {
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
};

bcm_port_t pon_port;
bcm_port_t nni_port;
bcm_vlan_t n_1_service_vsi;
int nof_n_1_services;
pon_up_ratelimit_service_info_s pon_n_1_service_info[6];
pon_llid_up_ratelimit_s pon_llid_up_ratelimit;

/* Field group for first level ratelimit */
bcm_field_group_t pon_up_ratelimit_grp;
bcm_field_group_t pon_up_ratelimit_grp_pri = 15;

/* Field group for second level ratelimit */
bcm_field_group_t pon_up_llid_ratelimit_grp;
bcm_field_group_t pon_up_llid_ratelimit_grp_pri = 16;

/* Create a field group to do the first rate limit(per VLAN with LLID). */
int pon_up_ratelimit_group_create(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;    

    BCM_FIELD_QSET_INIT(qset);          
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_group_create(unit, qset, pon_up_ratelimit_grp_pri, &pon_up_ratelimit_grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create_id $rv\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel1);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);
    rv = bcm_field_group_action_set(unit, pon_up_ratelimit_grp, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set $rv\n");
        return rv;
    }

    return rv;
}

/* Create and install field entries to do the first rate limit(per VLAN with LLID). */
int pon_up_ratelimit_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_field_entry_t entry;
    int idx;

    rv = pon_up_ratelimit_group_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in pon_ipv4_acl_group_create $rv\n");
        return rv;
    }

    rv = bcm_policer_create(unit, &pon_llid_up_ratelimit.pol_cfg, &pon_llid_up_ratelimit.policer_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_create with policer_id %d $rv\n", pon_llid_up_ratelimit.policer_id);
        print rv;
        return rv;
    }

    for (idx = 0; idx < nof_n_1_services; idx++) {
        rv = bcm_field_entry_create(unit, pon_up_ratelimit_grp, &entry);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_create $rv\n");
            return rv;
        }

        rv = bcm_field_qualify_InPort(unit, entry, pon_port, 0xffffffff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort with %d in_port $rv\n", pon_port);
            return rv;
        }
        
        rv = bcm_field_qualify_TunnelId(unit, entry, pon_n_1_service_info[idx].tunnel_id, 0x7ff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_TunnelId %d with in_port $rv\n", pon_n_1_service_info[idx].tunnel_id);
            return rv;
        }

        rv = bcm_field_qualify_OuterVlanId(unit, entry, pon_n_1_service_info[idx].up_ovlan, 0xfff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_OuterVlanId $rv\n");
            return rv;
        }

        rv = bcm_policer_create(unit, &pon_n_1_service_info[idx].pol_cfg, &pon_n_1_service_info[idx].policer_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_create with policer_id %d $rv\n", pon_n_1_service_info[idx].policer_id);
            print rv;
            return rv;
        }

        /* map traffic to meter pon_n_1_service_info[idx].policer_id in group=0 and meter pon_llid_up_ratelimit.policer_id in group=1 */
        rv = bcm_field_entry_policer_attach(unit, entry, 0, (pon_n_1_service_info[idx].policer_id & 0xffff));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_policer_attach with policer_id %d $rv\n", (pon_n_1_service_info[idx].policer_id & 0xffff));
            print rv;
            return rv;
        }

        rv = bcm_field_entry_policer_attach(unit, entry, 1, (pon_llid_up_ratelimit.policer_id & 0xffff));
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_policer_attach with policer_id %d $rv\n", (pon_llid_up_ratelimit.policer_id & 0xffff));
            print rv;
            return rv;
        }

        rv = bcm_field_entry_install(unit, entry);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR in bcm_field_entry_install $rv\n");
            print rv;
            return rv;
        }

        pon_n_1_service_info[idx].entry = entry;
    }

    return rv;
}

/* Destroy field entries and field group of the first rate limit(per VLAN with LLID). */
int pon_up_ratelimit_cleanup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_field_entry_t entry;
    int idx;

    rv = bcm_policer_destroy(unit, pon_llid_up_ratelimit.policer_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_policer_destroy with policer_id %d $rv\n", pon_llid_up_ratelimit.policer_id);
        print rv;
        return rv;
    }

    for (idx = 0; idx < nof_n_1_services; idx++) {
        entry = pon_n_1_service_info[idx].entry;
        
        rv = bcm_policer_destroy(unit, pon_n_1_service_info[idx].policer_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_policer_destroy with policer_id %d $rv\n", pon_n_1_service_info[idx].policer_id);
            print rv;
            return rv;
        }

        rv = bcm_field_entry_remove(unit, entry);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_remove returned $rv\n");
            return rv;
        }
        
        rv = bcm_field_entry_destroy(unit, entry);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_destroy returned $rv\n");
            return rv;
        }
    }

    rv = bcm_field_group_destroy(unit, pon_up_ratelimit_grp);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_destroy returned $rv\n");
        return rv;
    }

    return rv;
}

/* Init PP service models */
int pon_up_ratelimit_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    bcm_error_t rv = BCM_E_NONE;
    int idx;

    idx = 0;
    /* PON Port 4 Tunnel-ID 100 CVLAN 10 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 100;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 10;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;
    /* COLOR_BLIND meter from group=0 with CIR = 20M */ 
    bcm_policer_config_t_init(&pon_n_1_service_info[idx].pol_cfg);
    pon_n_1_service_info[idx].pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pon_n_1_service_info[idx].pol_cfg.ckbits_sec = 20000; 
    pon_n_1_service_info[idx].pol_cfg.ckbits_burst = 1000;
    pon_n_1_service_info[idx].pol_cfg.pkbits_burst = 0;
    pon_n_1_service_info[idx].pol_cfg.max_pkbits_sec = pon_n_1_service_info[idx].pol_cfg.ckbits_sec; 
    pon_n_1_service_info[idx].pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors mode */     

    idx++;
    /* PON Port 4 Tunnel-ID 100 CVLAN 20 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 100;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 20;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;
    /* COLOR_BLIND meter from group=0 with CIR = 40M */ 
    bcm_policer_config_t_init(&pon_n_1_service_info[idx].pol_cfg);
    pon_n_1_service_info[idx].pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pon_n_1_service_info[idx].pol_cfg.ckbits_sec = 40000; 
    pon_n_1_service_info[idx].pol_cfg.ckbits_burst = 1000;
    pon_n_1_service_info[idx].pol_cfg.pkbits_burst = 0;
    pon_n_1_service_info[idx].pol_cfg.max_pkbits_sec = pon_n_1_service_info[idx].pol_cfg.ckbits_sec; 
    pon_n_1_service_info[idx].pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors mode */     

    idx++;
    /* PON Port 4 Tunnel-ID 100 CVLAN 30 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 100;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 30;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;
    /* COLOR_BLIND meter from group=0 with CIR = 60M */ 
    bcm_policer_config_t_init(&pon_n_1_service_info[idx].pol_cfg);
    pon_n_1_service_info[idx].pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pon_n_1_service_info[idx].pol_cfg.ckbits_sec = 60000; 
    pon_n_1_service_info[idx].pol_cfg.ckbits_burst = 1000;
    pon_n_1_service_info[idx].pol_cfg.pkbits_burst = 0;
    pon_n_1_service_info[idx].pol_cfg.max_pkbits_sec = pon_n_1_service_info[idx].pol_cfg.ckbits_sec; 
    pon_n_1_service_info[idx].pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors mode */     

    idx++;
    /* PON Port 4 Tunnel-ID 100 CVLAN 40 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 100;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 40;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;
    /* COLOR_BLIND meter from group=0 with CIR = 100M */ 
    bcm_policer_config_t_init(&pon_n_1_service_info[idx].pol_cfg);
    pon_n_1_service_info[idx].pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pon_n_1_service_info[idx].pol_cfg.ckbits_sec = 100000; 
    pon_n_1_service_info[idx].pol_cfg.ckbits_burst = 1000;
    pon_n_1_service_info[idx].pol_cfg.pkbits_burst = 0;
    pon_n_1_service_info[idx].pol_cfg.max_pkbits_sec = pon_n_1_service_info[idx].pol_cfg.ckbits_sec; 
    pon_n_1_service_info[idx].pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors mode */     

    idx++;
    /* PON Port 4 Tunnel-ID 100 CVLAN 50 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 100;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 50;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;
    /* COLOR_BLIND meter from group=0 with CIR = 200M */ 
    bcm_policer_config_t_init(&pon_n_1_service_info[idx].pol_cfg);
    pon_n_1_service_info[idx].pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pon_n_1_service_info[idx].pol_cfg.ckbits_sec = 200000; 
    pon_n_1_service_info[idx].pol_cfg.ckbits_burst = 1000;
    pon_n_1_service_info[idx].pol_cfg.pkbits_burst = 0;
    pon_n_1_service_info[idx].pol_cfg.max_pkbits_sec = pon_n_1_service_info[idx].pol_cfg.ckbits_sec; 
    pon_n_1_service_info[idx].pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors mode */

    nof_n_1_services = idx + 1;

    /* COLOR_BLIND meter from group=1 with CIR = 200M */ 
    bcm_policer_config_t_init(&pon_llid_up_ratelimit.pol_cfg);
    pon_llid_up_ratelimit.pol_cfg.flags |= BCM_POLICER_MACRO;
    pon_llid_up_ratelimit.pol_cfg.ckbits_sec = 200000;
    pon_llid_up_ratelimit.pol_cfg.ckbits_burst = 1000;
    pon_llid_up_ratelimit.pol_cfg.pkbits_burst = BCM_POLICER_COLOR_BLIND;
    pon_llid_up_ratelimit.pol_cfg.max_pkbits_sec = pon_llid_up_ratelimit.pol_cfg.ckbits_sec; 
    pon_llid_up_ratelimit.pol_cfg.mode = bcmPolicerModeSrTcm;  /* single rate three colors mode */    
    pon_port = port_pon;
    nni_port = port_nni;
    
    rv = pon_app_init(unit, pon_port, nni_port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("pon_app_init failed $rv\n");
        return rv;
    }

    /* Drop all packets with DP 3 or above */
    rv = bcm_cosq_discard_set(unit, (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK));
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_discard_set failed $rv\n");
        return rv;
    }

    return rv;
}

/* Setup PP service models */
int pon_up_ratelimit_service_setup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;  
    int index;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;

    rv = vswitch_create(unit, &n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("uc vswitch_create failed $rv\n");
        return rv;
    }

    for (index = 0; index < nof_n_1_services; index++)    
    {
        pon_gport = 0;
        nni_gport = 0;
        service_mode = pon_n_1_service_info[index].service_mode;        
        tunnel_id    = pon_n_1_service_info[index].tunnel_id;        
        pon_lif_type = pon_n_1_service_info[index].up_lif_type;        
        up_ovlan     = pon_n_1_service_info[index].up_ovlan;        
        up_ivlan     = pon_n_1_service_info[index].up_ivlan;        
        up_pcp       = pon_n_1_service_info[index].up_pcp;        
        up_ethertype = pon_n_1_service_info[index].up_ethertype;        
        nni_lif_type = pon_n_1_service_info[index].down_lif_type;        
        down_ovlan   = pon_n_1_service_info[index].down_ovlan;        
        down_ivlan   = pon_n_1_service_info[index].down_ivlan; 
        
        /* Create PON LIF */        
        rv = pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport); 
        if (rv != BCM_E_NONE) {
            printf("pon_lif_create failed (%d) $rv\n", index);
            return rv;
        }
        /* Add PON LIF to vswitch */        
        rv = bcm_vswitch_port_add(unit, n_1_service_vsi, pon_gport);        
        if (rv != BCM_E_NONE) {
            printf("bcm_vswitch_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /*For downstream*/
        rv = multicast_vlan_port_add(unit, n_1_service_vsi+lif_offset, pon_port, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("multicast_vlan_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF ingress VLAN editor */       
        rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, pon_in_map_id_dft, NULL); 
        if (rv != BCM_E_NONE) {
            printf("pon_port_ingress_vt_set failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF egress VLAN editor */        
        rv = pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);   
        if (rv != BCM_E_NONE) {
            printf("pon_port_egress_vt_set failed (%d) $rv\n", index);
            return rv;
        }

        /*Only create one NNI LIF*/        
        if (index == 0) {                        
            /* Create NNI LIF */            
            rv = nni_lif_create(unit, nni_port, nni_lif_type, 0, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);
            if (rv != BCM_E_NONE) {
                printf("nni_lif_create failed (%d) $rv\n", index);
                return rv;
            }
            /*For upstream*/            
            rv = multicast_vlan_port_add(unit, n_1_service_vsi, nni_port, nni_gport); 
            if (rv != BCM_E_NONE) {
                printf("multicast_vlan_port_add failed (%d) $rv\n", index);
                return rv;
            }
            /* add to vswitch */            
            rv = bcm_vswitch_port_add(unit, n_1_service_vsi, nni_gport);            
            if (rv != BCM_E_NONE) {
                printf("bcm_vswitch_port_add failed (%d) $rv\n", index);
                return rv;
            }
            
            /* Set the multicast group offset to flood downstream packets in multicast group(n_1_service_vsi+lif_offset) */
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);
        } else {           
            nni_gport = pon_n_1_service_info[0].nni_gport;        
        }

        pon_n_1_service_info[index].pon_gport = pon_gport;        
        pon_n_1_service_info[index].nni_gport = nni_gport;
    }
    
    return rv;
}


/* Clean all PP service models */
int pon_up_ratelimit_service_cleanup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;
    int index;

    for (index = 0; index < nof_n_1_services; index++)
    {
        rv = vswitch_delete_port(unit, n_1_service_vsi, pon_n_1_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("vswitch_delete_port failed (%d) $rv\n", index);
            return rv;
        }
    }
    rv = vswitch_delete_port(unit, n_1_service_vsi, pon_n_1_service_info[0].nni_gport);
    if (rv != BCM_E_NONE) {
        printf("vswitch_delete_port failed (%d) $rv\n", 0);
        return rv;
    }
    
    rv = vswitch_delete(unit, n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("vswitch_delete failed $rv\n");
        return rv;
    }    
    
    return rv;
}


