/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_gal_o_pwe_o_mpls.c
 * Purpose: Example of using field processor to trap GACHoGALoPWEoLSPoETH packets to the CPU.
 *          The trapped packet should contain PWE InLif in the PPH.
 *
 *  The cint uses cint_mpls_lsr.c to create VSwitch with MPLS termination.
 *  GACHoGALoPWEoLSPoETH packet is trapped to the CPU with the PWE inLif,
 *  and the trap is mpls_unexpected_no_bos.
 * 
 *  Field processor is used to change the MPLS InLif to PWE Inlif.
 * 
 *  NOTE: soc property custom_feature_mpls_termination_check_bos_disable should be set.
 *    
 *  Usage:
 *  cint src/examples/dpp/cint_port_tpid.c 
 *  cint src/examples/dpp/cint_qos.c 
 *  cint src/examples/dpp/utility/cint_utils_l3.c   
 *  cint src/examples/dpp/cint_vswitch_vpls.c
 *  cint src/examples/dpp/cint_mpls_lsr.c
 *  cint src/examples/dpp/cint_advanced_vlan_translation_mode.c
 *  cint src/examples/dpp/cint_field_pwe_gal_lif_update.c
 *  cint src/examples/dpp/cint_gal_o_pwe_o_mpls.c
 *  cint
 *  int unit=0; int port1 = 13; int port2 = 14; 
 *  print gal_o_pwe_o_mpls_example(unit, port1, port2);
 * 
 *  When the packet of the following structure will be sent on port 1,
 *  it will be trapped to the cpu with the PWE InLif.
 *  
 * Headers:
 *  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- |
 *   |  | DA |SA||TIPD1 |Prio|VID|Prtcl-id|  MPLS    ||   PWE    ||GAL   ||GACH || OAM  ||
 *   |  |0:11|  ||0x8100|    |200|| 0x8847|Label:100 ||Label:2010||      ||     ||      ||
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-++-+-+-+-+-+-+|
 *   |            +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+          |
 *   |            | Figure 1: Packets Received from PWE1 to be trapped to CPU |          | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 */

/* Global configs */
int mpls_label = 100;
int cpu_port = 0;

/* 
 *  Add term entry to perform pop
 */
int
mpls_add_term_entry_ex(int unit)
{
    int rv;
    int mpls_termination_label_index_enable;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /* 
     * Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    
    /* incomming label */
    entry.label = mpls_label;

    /* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	if (mpls_termination_label_index_enable) {
		BCM_MPLS_INDEXED_LABEL_SET(&entry.label,mpls_label,2);
	}
    
    entry.qos_map_id = 0; /* qos not rellevant for BFD */
    
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}


/* 
 *  Add term entry to perform pop
 */
int
gal_o_pwe_o_mpls_example(int unit, int port_1, int port_2)
{

    int rv;
    bcm_rx_trap_config_t config; 
    int trap_id;
    bcm_rx_trap_t type;

    mpls_lsr_init(port_1, port_2, 0, 0, mpls_label, mpls_label, 0, 0 ,0);
    rv = vswitch_vpls_run_with_defaults_dvapi(unit, port_1, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_run_with_defaults_dvapi\n");
        return rv;
    }
    rv = mpls_add_term_entry_ex(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_term_entry_ex\n");
        return rv;
    }

    /* Disabling MplsUnknownLabel trap */
    type=bcmRxTrapMplsUnknownLabel;
    rv = bcm_rx_trap_type_create(unit, 0, type, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_type_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&config);
    config.trap_strength = 2;
    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set\n");
        return rv;
    }

    /* Configuring MplsUnexpectedNoBos trap to the CPU */
    type= bcmRxTrapMplsUnexpectedNoBos;
    rv = bcm_rx_trap_type_create(unit, 0, type, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_type_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&config);
    config.trap_strength=7;
    config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
    config.dest_port = cpu_port;
    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set\n");
        return rv;
    }

    /* Adding the field processor rules */
    rv = pwe_gal_lif_update_example(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in pwe_gal_lif_update_example\n");
        return rv;
    }

    return rv;
}

