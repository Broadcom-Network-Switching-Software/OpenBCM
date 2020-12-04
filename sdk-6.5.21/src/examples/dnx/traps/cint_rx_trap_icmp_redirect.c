/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_rx_trap_icmp_redirect.c
 * Purpose: Simple example how to set icmp redirect to cpu.
 *
 * cint 
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_icmp_redirect.c
 *
 * Main Function:
 *      cint_rx_trap_icmp_redirect_main(unit);
 * Destroy Function:
 *      cint_rx_trap_icmp_redirect_cleanup(unit);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_icmp_redirect.c
 * cint
 * cint_rx_trap_icmp_redirect_main(0);
 */

int cint_rx_trap_icmp_redirect_trap_id = -1;

 /**
 * \brief  
 *  Main function for bcmSwitchIcmpRedirectToCpu configuration
 * \par DIRECT INPUT: 
 *   \param [in] unit      -  Unit Id
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT: 
 *   int -  Error Type
 * \par INDIRECT OUTPUT: 
 *   * None
 * \remark 
 *   * None
 * \see
 *   * None
 */
int cint_rx_trap_icmp_redirect_main(
int unit,
int fwd_strength)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_config_t_init(&config);
    config.trap_strength = fwd_strength;
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_LOCAL_CPU;
    
    rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrapIcmpRedirect, &config, &cint_rx_trap_icmp_redirect_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error in cint_utils_rx_trap_create_and_set(%s) \n", bcm_errmsg(rv));
    }  
    
    return rv;    
}

/**
* \brief  
*  Cleanup of cint_rx_trap_icmp_redirect_main configuration
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int -  Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int cint_rx_trap_icmp_redirect_destroy(
int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_icmp_redirect_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy(%s) \n", bcm_errmsg(rv));
    }

    return rv;
}

/**
 * Configuration of route with InLIF=OutLIF, packet will be routed from in_port to out-port
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f (192.255.255.0-192.255.255.15)
 */
int cint_rx_trap_route_with_inlif_equals_outlif(
    int unit,
    int in_port,
    int out_port)
{
    int rv = BCM_E_NONE;
    int host_format = 0;
    int is_arp_plus_ac = 0;
    bcm_failover_t failover_id = 0;
    int failover_out_port = 0;
    int do_raw = 0;
    int kaps_result = 0xB006;    
    int intf_in = 100;          /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    /*
     * Set 'fec' as on the coresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw
     */
    int fec;
    int vrf = 1;
    int encap_id = 8193;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100;
    int failover_is_primary = 0;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;

    proc_name = "cint_rx_trap_route_with_inlif_equals_outlif";
    printf("%s(): Enter. in_port %d out_port %d do_raw %d host_format %d kaps_result 0x%08X\r\n",proc_name, in_port, out_port, do_raw, host_format,kaps_result);
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &kaps_result);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = 0x1384;

        /*
         * Jr2 myMac assignment is more flexible than in Jer1
         */
        intf_out_mac_address[0] = 0x00;
        intf_out_mac_address[1] = 0x12;
        intf_out_mac_address[2] = 0x34;
        intf_out_mac_address[3] = 0x56;
        intf_out_mac_address[4] = 0x78;
        intf_out_mac_address[5] = 0x9a;
    }
    fec = kaps_result;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }
    if (failover_id != 0)
    {
        rv = out_port_set(unit, failover_out_port);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, out_port_set failover_out_port %d\n",proc_name, failover_out_port);
            return rv;
        }
    }
    
    /*
     * 3. Create ETH-RIF and set its properties 
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    
    /*
     * 4. Set Incoming ETH-RIF properties 
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */
    if (is_arp_plus_ac)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    }
    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    if (is_arp_plus_ac)
    {
        /*
         * for arp+ac, bcm_vlan_port_translation_set is the API which sets the AC part in
         * the ARP+AC entry in EEDB
         */
        printf("%s(): We have arp_plus_ac. Call bcm_vlan_port_translation_set()\n",proc_name);
        bcm_vlan_port_translation_t lif_translation_info;
        bcm_vlan_port_translation_t_init(&lif_translation_info);
        int arp_outlif;
        int vlan_edit_profile = 7;
        bcm_gport_t arp_ac_gport;

        arp_outlif = BCM_L3_ITF_VAL_GET(encap_id);
        BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, arp_outlif);
        BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);

        /*
        * Perform vlan editing:
        */
        rv = vlan_translate_ive_eve_translation_set(unit, arp_ac_gport,         /* lif  */
                                                          0x9100,               /* outer_tpid */
                                                          0x8100,               /* inner_tpid */
                                                          bcmVlanActionArpVlanTranslateAdd,  /* outer_action */
                                                          bcmVlanActionNone,                 /* inner_action */
                                                          vlan,                 /* new_outer_vid */
                                                          2*vlan,               /* new_inner_vid */
                                                          vlan_edit_profile,    /* vlan_edit_profile */
                                                          0,                    /* tag_format - must be untag! */
                                                          FALSE                 /* is_ive */
                                                          );

        /*
        * Check that the VLAN Edit info was set correctly:
        */
        bcm_vlan_port_translation_t lif_translation_info_get;
        lif_translation_info_get.flags = BCM_VLAN_ACTION_SET_EGRESS;
        lif_translation_info_get.gport = arp_ac_gport;
        
        lif_translation_info_get.new_outer_vlan = 0xFFFFFFFF;
        lif_translation_info_get.new_inner_vlan = 0xFFFFFFFF;
        lif_translation_info_get.vlan_edit_class_id = 0xFFFFFFFF;

        rv = bcm_vlan_port_translation_get(unit, &lif_translation_info_get);

        if (rv != BCM_E_NONE) {
            printf("%s(): Error, lif_translation_info_get return error = %d\n", proc_name, rv);
            return rv;
        }

        if (lif_translation_info_get.new_outer_vlan != vlan) {
            printf("%s(): Error, get.new_outer_vlan = %d  set.new_outer_vlan = %d\n", proc_name, 
                   lif_translation_info_get.new_outer_vlan, vlan);

            return BCM_E_INTERNAL;
        }

        if (lif_translation_info_get.new_inner_vlan != 2*vlan) {
            printf("%s(): Error, get.new_inner_vlan = %d  set.new_inner_vlan = %d\n", proc_name, 
                   lif_translation_info_get.new_inner_vlan, 2*vlan);

            return BCM_E_INTERNAL;
        }

        if (lif_translation_info_get.vlan_edit_class_id != vlan_edit_profile) {
            printf("%s(): Error, get.vlan_edit_class_id = %d  set.vlan_edit_class_id = %d\n", proc_name, 
                   lif_translation_info_get.vlan_edit_class_id, vlan_edit_profile);

            return BCM_E_INTERNAL;
        }
    }

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    if (host_format != 2)
    {
        printf("%s(): host_format %d. Create main FEC and set its properties.\r\n",proc_name, host_format);
        BCM_GPORT_LOCAL_SET(gport, out_port);
        rv = l3__egress_only_fec__create_inner(unit, fec, ((host_format == 1) || (host_format == 3) ) ? 0 : intf_out, encap_id, gport, 0,
                                               failover_id, failover_is_primary,&encoded_fec);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object FEC only\n",proc_name);
            return rv;
        }
        printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);
        /*
         * if protection is enabled , also create another fec(primary)
         */
        if (failover_id != 0)
        {
            fec--;
            printf("%s(): host_format %d failover_id %d. Create main FEC and set its properties.\r\n",proc_name, host_format, failover_id);
            BCM_GPORT_LOCAL_SET(gport, failover_out_port);
            failover_is_primary = 1;
            rv = l3__egress_only_fec__create_inner(unit, fec, (host_format == 1) ? 0 : intf_out, encap_id, gport, 0,
                                                   failover_id, failover_is_primary,&encoded_fec);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, create egress object FEC only\n",proc_name);
                return rv;
            }
            printf("%s(): Failover, fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);
        }
    }

    /*
     * 7. Add Route entry
     */
    printf("%s(): Add route entry. Raw %d. route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, do_raw, route, mask, vrf, fec);
    if (do_raw != 0)
    {
        rv = add_route_ipv4_raw(unit, route, mask, vrf, fec);
        err_proc_name = "add_route_ipv4_raw";
    }
    else
    {
        rv = add_route_ipv4(unit, route, mask, vrf, fec);
        err_proc_name = "add_route_ipv4";
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function %s(), \n",proc_name,err_proc_name);
        return rv;
    }
    
    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    if (host_format == 1)
    {
        rv = add_host_ipv4(unit, host, vrf, _l3_itf, intf_out, 0);
    }
    else if (host_format == 2)
    {
        rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);
    }
    else
    {
        rv = add_host_ipv4(unit, host, vrf, _l3_itf, 0, 0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

