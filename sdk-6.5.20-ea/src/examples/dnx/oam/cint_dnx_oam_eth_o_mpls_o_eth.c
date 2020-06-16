/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_oam_eth_o_mpls_o_eth.c Purpose: OAM CFMoEthoMplsoEth functions.
 */



/**
 * Create P2P AC to VPLS setting and configure OAM MIP on AC
 * Set a trap action on egress side.
 *  
 * @param unit
 * @param ac_port
 * @param pwe_port
 * 
 * @return int
 */
 int dnx_oam_eth_o_mpls_o_eth_run_p2p_mip(int unit, int ac_port , int pwe_port, int dest_port, int opcode)
 {
    int rv = BCM_E_NONE;
    unsigned int flags = 0;
    int ing_trap_code;
    int egr_trap_code;
    int trap_gport;
    bcm_rx_trap_config_t trap_config;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    bcm_oam_profile_t ingress_profile_id = 1;
    bcm_oam_profile_t egress_profile_id = 1;
    bcm_oam_endpoint_info_t mip_info;
    bcm_oam_group_info_t group_info;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
    bcm_gport_t egress_ac;
    bcm_oam_profile_t oam_profile;
   
    /*1, create ac <--> pwe service*/
    rv = vpls_pwe_tagged_mode_p2p_basic_main(unit,ac_port,pwe_port);
    if (rv != BCM_E_NONE)
    {
       printf("Error in  create ac <--> pwe service fail.\n");
       return rv;
    }
    
    printf("Creating group\n");
    bcm_oam_group_info_t_init(&group_info);
    group_info.id = 7;
    group_info.flags = BCM_OAM_GROUP_WITH_ID;
    sal_memcpy(group_info.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    egress_ac = cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id;
    
    /* Create OAM action profiles */
    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE)
    {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    oam_profile = oam_lif_profiles.oam_profile_eth_single_ended;
    
    /**
     * Get default profile encoded values.
     */
    rv = bcm_oam_profile_id_get_by_type(unit, oam_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_oam_profile_id_get_by_type(ingress)\n");
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, oam_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_oam_profile_id_get_by_type(egress)\n");
        return rv;
    }
    /*Bind lif with ingress and egress profiles*/
    bcm_oam_lif_profile_set(unit, 0, egress_ac, ingress_profile_id, egress_profile_id);
    
    /*
    * Adding a MIP
    */
    bcm_oam_endpoint_info_t_init(&mip_info);
    mip_info.type = bcmOAMEndpointTypeEthernet;
    mip_info.group = group_info.id;
    mip_info.level = 3;
    mip_info.gport = egress_ac;
    mip_info.flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;
  
    printf("bcm_oam_endpoint_create mip_info\n");
    rv = bcm_oam_endpoint_create(unit, &mip_info);
    if (rv != BCM_E_NONE)
    {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    printf("Created MIP with id %d\n", mip_info.id);
    
    /* Create a new trap */
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &ing_trap_code);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_type_create.\n");
       return rv;
    }

    /* Setup the new trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
    trap_config.trap_strength = 0;
    trap_config.dest_port = dest_port;
    rv = bcm_rx_trap_set(unit, ing_trap_code, trap_config);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_set.\n");
       return rv;
    }
    /*ingress profile action set */
    rv = dnx_oam_profile_action_set_tcl(unit, 0/*bcmOAMMatchTypeMIP*/, 1, 0/*bcmOAMDestMacTypeMcast*/,0, ing_trap_code,ingress_profile_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error in dnx_oam_profile_action_set_tcl.\n");
      return rv;
    }

    /* Configure egress trap in case of egress action set */
    rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapEgTxOamUpMEPDest1, &egr_trap_code);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_type_get: (%s)\n", bcm_errmsg(rv));
       return rv;
    }

    /* Configure the trap */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stamped_trap_code = 0xe0;
    /* Encode trap, trap strength and snoop strength as a gport */
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, ing_trap_code, 7, 0);
    rv = bcm_rx_trap_set(unit, egr_trap_code, trap_config);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_set: (%s)\n", bcm_errmsg(rv));
       return rv;
    }
    /*egress profile action set */
    rv = dnx_oam_profile_action_set_tcl(unit, 0/*bcmOAMMatchTypeMIP*/, 1,  0/*bcmOAMDestMacTypeMcast*/,0, egr_trap_code,egress_profile_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error in dnx_oam_profile_action_set_tcl.\n");
      return rv;
    }

    return rv;

}


