/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_oam_y1731_over_pwe_protection.c
 *
 * Purpose: basic example for oam over pwe protection.
 *
 * Examples:
 *   - 1. Example of oamp injection with PWE protection
 *   - 2. Example of Hierarchial LM (HLM) in egress with PWE protection
 */

/*
 * 1. Example of oamp injection with PWE protection
 *
 * Purpose:
 *   On J2C, Q2A and above device, packet injected from oamp can ignore protection state.
 *   In this example, we verified if device support ignoring protection state.
 *
 * Objects to be created:
 *   A pwe protected model is setup, and oam group, lif proflie and 2 ACC endpoints are created,
 *   including one PWE endpoint for primary, and one PWE endpoint for backup,
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../src/./examples/dnx/oam/cint_oam_action.c
 * cint ../../src/./examples/sand/cint_vpls_mp_basic.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_y1731_over_pwe_protection.c
 * cint
 * cint_vpls_pwe_protection_info.cw_present=1;
 *
 * Step 1. Load cint_vpls_pwe_protection_info to setup pwe protection model, and
 *         create oam group, Lif profile and two ACC MEPs. One for PWE primary, another for PWE backup
 * oam_run_with_pwe_protection(0,200,201);
 * exit;
 *
 * Step 2. Test without ignoring protection state HW capability, and switch to backup pwe, then primary pwe
 * cint
 * vpls_pwe_protection_basic_set_failover_ignore(0,0);
 * exit;
 *
 * Step 2.1 Switch to backup pwe
 * cint
 * vpls_pwe_protection_basic_set_failover(0,0);
 * exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Step 2.2 Switch to primary pwe
 * cint
 * vpls_pwe_protection_basic_set_failover(0,1);
 * exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Step 3. Test with ignoring protection state HW capability, if device support
 *     Note: currently, J2C, Q2A and above support
 * cint
 * vpls_pwe_protection_basic_set_failover_ignore(0,1);
 * exit;
 *
 * Step 3.1 Switch to backup pwe
 * cint
 * vpls_pwe_protection_basic_set_failover(0,0);
 * exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Step 3.2 Switch to primary pwe
 * cint
 * vpls_pwe_protection_basic_set_failover(0,1);
 * exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 */

/*
 * 2. Example of HLM in egress with PWE protection
 *
 * Purpose:
 *   In this example, HLM in egress with PWE protection is verified.
 *   Expect correct counter was increased after injecting data, CCM, DMM and LMM packets
 *
 * Objects to be created:
 *   A pwe protected model is setup, and
 *   oam group, lif proflie and 6 ACC endpoints are created,
 *   including one PWE endpoint for primary, one PWE endpoint for backup,
 *   one MPLS-TP endpoint for primary, one MPLS-TP endpoint for backup,
 *   one MPLS-Section endpoint for primary and one MPLS-Section endpoint for backup.
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/cint_vpls_mp_basic.c
 * cint ../../src/./examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/oam/cint_oam_action.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_y1731_over_pwe_protection.c
 * cint
 * cint_vpls_pwe_protection_info.cw_present=1;
 * is_hlm_egress_with_section=1;
 *
 * Step 1. Load cint_vpls_pwe_protection_info to setup pwe protection model, and
 *         create oam group, Lif profile and two ACC MEPs. One for PWE primary, another for PWE backup
 * oam_run_hlm_egress_with_pwe_protection(0,200,201);
 * exit;
 *
 * Step 2. injected data packet
 * Step 2.1 injected data packet to backup mpls tunnel
 *          expected counter for backup MPLS-TP and MPLS-Section are increased
 *
 * Step 2.2 injected data packet to backup pwe
 *          expected counter for backup PWE, MPLS-TP and MPLS-Section are increased
 *
 * Step 2.3 injected data packet to primary mpls tunnel
 *          expected counter for primary MPLS-TP and MPLS-Section are increased
 *
 * Step 2.4 injected data packet to primary pwe
 *          expected counter for primary PWE, MPLS-TP and MPLS-Section are increased
 *
 *
 * Step 3. injected ccm packet
 * Step 3.1 injected ccm packet from backup MPLS-Section endpoint
 *          expected counter for backup MPLS-Section are increased
 *
 * Step 3.2 injected ccm packet from backup MPLS-TP endpoint
 *          expected counter for backup MPLS-TP and MPLS-Section are increased

 * Step 3.3 injected ccm packet from backup PWE endpoint
 *          expected counter for backup PWE, MPLS-TP and MPLS-Section are increased
 *
 * Step 3.4 injected ccm packet from primary MPLS-Section endpoint
 *          expected counter for primary MPLS-Section are increased
 *
 * Step 3.5 injected ccm packet from primary MPLS-TP endpoint
 *          expected counter for primary MPLS-TP and MPLS-Section are increased

 * Step 3.6 injected ccm packet from primary PWE endpoint
 *          expected counter for primary PWE, MPLS-TP and MPLS-Section are increased
 *
 *
 * Step 4. injected dmm packet
 * Step 4.1 injected dmm packet from backup MPLS-Section endpoint
 *          expected no counter is increased
 *
 * Step 4.2 injected dmm packet from backup MPLS-TP endpoint
 *          expected counter for backup MPLS-Section are increased
 *
 * Step 4.3 injected dmm packet from backup PWE endpoint
 *          expected counter for backup MPLS-TP and MPLS-Section are increased
 *
 * Step 4.4 injected dmm packet from primary MPLS-Section endpoint
 *          expected no counter is increased
 *
 * Step 4.5 injected dmm packet from primary MPLS-TP endpoint
 *          expected counter for primary MPLS-Section are increased
 *
 * Step 4.6 injected dmm packet from primary PWE endpoint
 *          expected counter for primary MPLS-TP and MPLS-Section are increased
 *
 *
 * Step 5. injected lmm packet
 * Step 5.1 injected lmm packet from backup MPLS-Section endpoint
 *          expected no counter is increased, and TxFCf of LMM packet is same as counter for backup MPLS-Section endpoint.
 *
 * Step 5.2 injected lmm packet from backup MPLS-TP endpoint
 *          expected counter for backup MPLS-Section are increased, and TxFCf of LMM packet is same as counter for backup MPLS-TP endpoint.
 *
 * Step 5.3 injected lmm packet from backup PWE endpoint
 *          expected counter for backup MPLS-TP and MPLS-Section are increased, and TxFCf of LMM packet is same as counter for backup PWE endpoint.
 *
 * Step 5.4 injected lmm packet from primary MPLS-Section endpoint
 *          expected no counter is increased, and TxFCf of LMM packet is same as counter for primary MPLS-TP endpoint.
 *
 * Step 5.5 injected lmm packet from primary MPLS-TP endpoint
 *          expected counter for primary MPLS-Section are increased, and TxFCf of LMM packet is same as counter for primary MPLS-TP endpoint.
 *
 * Step 5.6 injected lmm packet from primary PWE endpoint
 *          expected counter for primary MPLS-TP and MPLS-Section are increased, and TxFCf of LMM packet is same as counter for primary PWE endpoint.
 */

bcm_oam_group_info_t    group_info_short_ma;

bcm_oam_endpoint_info_t mep_acc_pwe_bp_info;
bcm_oam_endpoint_info_t mep_acc_pwe_pri_info;
bcm_oam_endpoint_info_t mep_acc_mpls_bp_info;
bcm_oam_endpoint_info_t mep_acc_mpls_pri_info;
bcm_oam_endpoint_info_t mep_acc_section_bp_info;
bcm_oam_endpoint_info_t mep_acc_section_pri_info;

int lm_counter_if_pwe     = 0;
int lm_counter_if_mpls    = 1;
int lm_counter_if_section = 2;
int lm_counter_base_id_pwe_pri = 0;
int lm_counter_base_id_pwe_bp  = 0;
int lm_counter_base_id_mpls_pri = 0;
int lm_counter_base_id_mpls_bp  = 0;
int lm_counter_base_id_section_pri = 0;
int lm_counter_base_id_section_bp  = 0;

int lm_pwe_port = 0;

bcm_mac_t pwe_mac_addr_bp  = {0x00, 0x00, 0x00, 0x00, 0x01, 0x17};
bcm_mac_t pwe_mac_addr_pri = {0x00, 0x00, 0x00, 0x00, 0x01, 0x18};

int is_dual_ended_lm = 0;
int is_hlm_egress_with_section = 0;

int is_oam_profile_created = 0;

/**
 * DNX ONLY.
 * Check if device support vpls model with pwe protection for oam testing
 *
 * @param unit
 *
 * @return int
 */
int
vpls_pwe_protection_check_support(
    int unit)
{
    int rv = BCM_E_NONE;

    if (!is_device_or_above(unit, JERICHO2))
    {
        printf("Only Jericho2 and above are supported\n");
        return BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Failover Set
 */
int
vpls_pwe_protection_basic_set_failover (
    int unit,
    int is_primary)
{
    int rv;
    bcm_failover_t failover_id, egress_failover_id;
    int egress_enable;

    failover_id = cint_vpls_basic_info.pwe_failover_id_fec;
    egress_failover_id = cint_vpls_basic_info.pwe_failover_id_outlif;

    rv = bcm_failover_set(unit, failover_id, is_primary);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_failover_set failover_id:  0x%08x \n", rv, failover_id);
        return rv;
    }

    rv = bcm_failover_set(unit, egress_failover_id, is_primary);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_failover_set failover_id:  0x%08x \n", rv, egress_failover_id);
        return rv;
    }

    return rv;
}

int
vpls_pwe_protection_basic_set_failover_ignore (
    int unit,
    int is_ignore)
{
    int rv;
    int protection_state_ignore_support = *(dnxc_data_get(unit, "device", "general", "protection_state_ignore", NULL));

    if (!protection_state_ignore_support)
    {
        printf("Ignore Failover state is not supported by this device\n", rv);
        return BCM_E_UNAVAIL;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchControlOamBfdFailoverStateIgnore, is_ignore);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcmSwitchControlOamBfdFailoverStateIgnore is_ignore: %d \n", rv, is_ignore);
        return rv;
    }

    return rv;
}


/**
 * DNX ONLY.
 * Configure ingress and egress profiles for MPLS or PWE LIFs.
 * Use profiles created in cint_oam_action.c. *
 *
 * @param unit
 * @param gport - MPLS/PWE In-LIF
 * @param mpls_out_gport - MPLS/PWE Out-LIF
 * @param is_dual_ended_lm - Used for Dual-Ended LM profile configuration
 * @param mep_info - Used for setting mep information
 *
 * @return int
 */
int config_oam_mpls_or_pwe_profile(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t mpls_out_gport,
    int is_dual_ended_lm,
    bcm_oam_endpoint_info_t *mep_info)
{
    int rv;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile_id = 1, acc_ingress_profile = 1, egress_profile_id = BCM_OAM_PROFILE_INVALID;

    if(mep_info->type==bcmOAMEndpointTypeBHHMPLS || mep_info->type==bcmOAMEndpointTypeBHHPweGAL)
    {
        /** Profiles for MPLS-TP */
        if (is_dual_ended_lm)
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_mpls_dual_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_mpls_dual_ended;
        }
        else
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_mpls_single_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_mpls_single_ended;
        }
    }
    else
    {
        /** Profiles for PWE */
        if (is_dual_ended_lm)
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_pwe_dual_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_pwe_dual_ended;
        }
        else
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_pwe_single_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_pwe_single_ended;
        }
    }

    mep_info->acc_profile_id = acc_ingress_profile;

    if (gport != BCM_GPORT_INVALID)
    {
        flags = 0;
        printf("Gport is 0x%08X\n", gport);
        rv = bcm_oam_lif_profile_set(unit, flags, gport, ingress_profile_id, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
            return rv;
        }
    }

    if (mpls_out_gport != BCM_GPORT_INVALID)
    {
        /** Create egress profile - profile is used, but no actions are performed. */
        egress_profile_id = 1;
        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_oam_profile_id_get_by_type(egress) \n");
            return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, flags, mpls_out_gport, BCM_OAM_PROFILE_INVALID, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
            return rv;
        }
    }

    return rv;
}

/**
 * DNX ONLY.
 * Configure ingress and egress profiles for MPLS Section LIF.
 * Use cint_oam_action.c to create action profiles for OAM
 * section.
 *
 *
 * @param unit
 * @param gport - MPLS In-LIF
 * @param mpls_out_gport - MPLS/PWE Out-LIF
 * @param is_dual_ended_lm - Used for Dual-Ended LM profile configuration
 * @param mep_info - Used for setting mep information
 *
 * @return int
 */
int config_oam_section_profile(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t mpls_out_gport,
    int is_dual_ended_lm,
    bcm_oam_endpoint_info_t *mep_info)
{
    int rv;
    int lm_type;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile_id = 1, acc_ingress_profile = 1, egress_profile_id = BCM_OAM_PROFILE_INVALID;

    /** Set lm type */
    lm_type = is_dual_ended_lm ? 1 : 0;

    /** Create ingress oam section profile */
    rv = oam_section_action_profile_create(unit,lm_type,&ingress_profile_id,&acc_ingress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    mep_info->acc_profile_id = acc_ingress_profile;
    if (gport != BCM_GPORT_INVALID)
    {
        flags = 0;
        rv = bcm_oam_lif_profile_set(unit, flags, gport, ingress_profile_id, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
           return rv;
        }
    }

    if (mpls_out_gport != BCM_GPORT_INVALID)
    {
        /** Create egress profile - profile is used, but no actions are performed. */
        egress_profile_id = 1;
        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("bcm_oam_profile_id_get_by_type(egress) \n");
           return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, flags, mpls_out_gport, BCM_OAM_PROFILE_INVALID, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
           return rv;
        }
    }

  return rv;
}

/**
 * DNX ONLY.
 * Create MEP for PWE or MPLS-TP or MPLS-Section, based on mep_type
 */
int
oam_mep_over_tunnel_create (
    int unit,
    bcm_oam_endpoint_info_t *mep_acc_info,
    int mep_type,
    int group,
    int counter_base,
    int counter_if,
    int gport,
    int out_gport,
    int mpls_tunnel_id,
    int tx_gport,
    int is_mep_with_id,
    int mep_id,
    int mdl,
    int is_dual_ended_lm,
    int is_primary)
{
    bcm_error_t rv;
    bcm_oam_loss_t loss_info;
    bcm_oam_delay_t delay_info;
    int loss_id = 0;

    /* 1. Create OAM MPLS/PWE MEP */
    bcm_oam_endpoint_info_t_init(mep_acc_info);
    mep_acc_info->type = mep_type;
    mep_acc_info->endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;

    /*RX*/
    mep_acc_info->group = group;
    mep_acc_info->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    mep_acc_info->level = mdl;     /*Y1731 level*/
    mep_acc_info->gport = gport; /* in lif */
    mep_acc_info->mpls_out_gport = out_gport; /* out lif */

    /*TX*/
    if (tx_gport){
        BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info->tx_gport, tx_gport); /* port that the traffic will be transmitted on */
    }
    mep_acc_info->name = is_primary ? 0x1234 : 0x1235;
    mep_acc_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;

    mep_acc_info->intf_id = mpls_tunnel_id;
    mep_acc_info->egress_label.label = 3000;  /* Constant label for PWE tunneling */
    mep_acc_info->egress_label.ttl = 0x1e;
    mep_acc_info->egress_label.exp = 1;

    if (bcmOAMEndpointTypeBhhSection == mep_type) {
        rv = config_oam_section_profile(unit, gport, out_gport, is_dual_ended_lm, mep_acc_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in config_oam_section_profile.\n", rv);
            return rv;
        }
    } else {
        rv = config_oam_mpls_or_pwe_profile(unit, gport, out_gport, is_dual_ended_lm, mep_acc_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in config_oam_mpls_or_pwe_profile.\n", rv);
            return rv;
        }
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, mep_acc_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }

    /* 2. Add loss on MEP */
    bcm_oam_loss_t_init(&loss_info);
    loss_info.id = mep_acc_info->id;
    if (is_dual_ended_lm) {
        loss_info.flags = BCM_OAM_LOSS_WITH_ID;
    } else {
        loss_info.flags = BCM_OAM_LOSS_WITH_ID | BCM_OAM_LOSS_SINGLE_ENDED;
    }

    if (bcmOAMEndpointTypeBHHPwe == mep_type) {
        loss_id = 65552;
    } else if (bcmOAMEndpointTypeBHHMPLS == mep_type) {
        loss_id = 65556;
    } else {
        loss_id = 65560;
    }
    if (is_primary) {
        loss_id += 12;
    }
    loss_info.loss_id = loss_id;

    loss_info.period = 0;
    printf("bcm_oam_loss_add loss_info\n");
    rv = bcm_oam_loss_add(unit, &loss_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_loss_add\n", rv);
        return rv;
    }

    /* 3. Add delay on MEP */
    bcm_oam_delay_t_init(&delay_info);
    delay_info.id = mep_acc_info->id;
    delay_info.flags = 0;
    delay_info.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    delay_info.period = 0;
    printf("bcm_oam_delay_add delay_info\n");
    rv = bcm_oam_delay_add(unit, &delay_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_delay_add\n", rv);
        return rv;
    }

    return rv;
}

/*
 * 1. Example of oamp injection with PWE protection
 */
int
oam_run_with_pwe_protection(
    int unit,
    int ac_port,
    int pwe_port)
{
    bcm_error_t rv;

    bcm_oam_group_info_t *group_info;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
    bcm_oam_endpoint_info_t mep_acc_info;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;

    int is_primary;
    int nof_objects = 2;

    /*
     * Setup VPLS PWE protection model
     */
    pwe_protection_with_one_fec = 1;

    rv = vpls_mp_basic_main(unit, ac_port, pwe_port);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in vpls_mp_basic_main\n", rv);
        return rv;
    }

    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    group_info = &group_info_short_ma;
    printf("Created group short name format\n");

    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    /*
     * Adding two acc MEPs, one for PWE backup, another for PWE primary
     */
    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        gport = is_primary ? cint_vpls_basic_info.mpls_port_id_ingress_primary : cint_vpls_basic_info.mpls_port_id_ingress;
        mpls_tunnel_gport = is_primary ? cint_vpls_basic_info.mpls_encap_fec_id_primary : cint_vpls_basic_info.mpls_encap_fec_id;
        BCM_L3_ITF_SET(mpls_tunnel_gport, BCM_L3_ITF_TYPE_FEC, mpls_tunnel_gport);
        mpls_out_gport = is_primary ? cint_vpls_basic_info.mpls_port_id_egress_primary : cint_vpls_basic_info.mpls_port_id_egress;

        bcm_oam_endpoint_info_t_init(&mep_acc_info);
        mep_acc_info.type = bcmOAMEndpointTypeBHHPwe;
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;

        /** Lif Profile */
        rv = config_oam_mpls_or_pwe_profile(unit, gport, mpls_out_gport, 0, &mep_acc_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error rv(%d), in config_oam_mpls_or_pwe_profile.\n", rv);
            return rv;
        }

        /*RX*/
        mep_acc_info.group = group_info->id;
        mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
        mep_acc_info.level = 7;     /*Y1731 level*/
        mep_acc_info.gport = gport; /* in lif */
        mep_acc_info.mpls_out_gport = mpls_out_gport; /* out lif */


        /*TX*/
        mep_acc_info.tx_gport = BCM_GPORT_INVALID;
        mep_acc_info.name = is_primary ? 0x1234 : 0x1235;
        mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;

        mep_acc_info.intf_id = mpls_tunnel_gport;
        mep_acc_info.egress_label.label = 3000;  /* Constant label for PWE tunneling */
        mep_acc_info.egress_label.ttl = 0xa;
        mep_acc_info.egress_label.exp = 1;

        printf("bcm_oam_endpoint_create mep_acc_info\n");
        rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
            return rv;
        }

        if (is_primary)
        {
            sal_memcpy(&mep_acc_pwe_pri_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for pwe primary with id %d\n", mep_acc_info.id);
        }
        else
        {
            sal_memcpy(&mep_acc_pwe_bp_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for pwe backup with id %d\n", mep_acc_info.id);
        }
    }

    return rv;
}

/*
 * Add l2 addr for data traffic to pwe.
 */
int
oam_run_hlm_egress_pwe_l2_addr_add (
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;
    int is_primary = 0;
    int nof_objects = 2;

    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        bcm_l2_addr_t_init(&l2addr, (is_primary?pwe_mac_addr_pri:pwe_mac_addr_bp), cint_vpls_basic_info.vpn);
        l2addr.flags    = BCM_L2_STATIC;
        l2addr.port     = lm_pwe_port;
        l2addr.encap_id = is_primary ? cint_vpls_basic_info.encap_id_primary : cint_vpls_basic_info.encap_id;

        rv = bcm_l2_addr_add(unit, l2addr);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_l2_addr_add\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * Create 2 PWE MEPs for HLM in egress with PWE protection.
 */
int
oam_run_hlm_egress_pwe_mep_create (
    int unit,
    int is_mep_with_id)
{
    bcm_error_t rv;
    bcm_oam_endpoint_info_t mep_acc_info;
    int mep_id = 0;
    int mep_type;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;
    int tx_port;
    int counter_base;
    int counter_if;
    int mdl = 7;
    int is_primary;
    int nof_objects = 2;

    counter_if = lm_counter_if_pwe;
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_pwe_pri);
    BCM_IF_ERROR_RETURN(rv);
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_pwe_bp);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Adding two acc MEPs, one for PWE backup, another for PWE primary
     */
    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        mep_type = bcmOAMEndpointTypeBHHPwe;
        gport = BCM_GPORT_INVALID;
        mpls_out_gport = is_primary ? cint_vpls_basic_info.mpls_port_id_egress_primary : cint_vpls_basic_info.mpls_port_id_egress;
        mpls_tunnel_gport = is_primary ? cint_vpls_basic_info.mpls_encap_fec_id_primary : cint_vpls_basic_info.mpls_encap_fec_id;
        BCM_L3_ITF_SET(mpls_tunnel_gport, BCM_L3_ITF_TYPE_FEC, mpls_tunnel_gport);
        tx_port = 0;
        counter_if = lm_counter_if_pwe;
        counter_base = is_primary ? lm_counter_base_id_pwe_pri : lm_counter_base_id_pwe_bp;
        mep_id = is_mep_with_id ? (is_primary ? mep_acc_pwe_pri_info.id : mep_acc_pwe_bp_info.id) : 0;
        rv = oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info_short_ma.id, counter_base, counter_if,
                 gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
                 is_mep_with_id, mep_id, mdl, is_dual_ended_lm, is_primary);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
            return rv;
        }

        if (is_primary)
        {
            sal_memcpy(&mep_acc_pwe_pri_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for PWE primary with id %d\n", mep_acc_info.id);
        }
        else
        {
            sal_memcpy(&mep_acc_pwe_bp_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for PWE backup with id %d\n", mep_acc_info.id);
        }
    }

    return rv;
}

/*
 * Create 2 PWE MEPs for HLM in egress with PWE protection.
 */
int
oam_run_hlm_egress_mpls_tp_mep_create (
    int unit,
    int is_mep_with_id)
{
    bcm_error_t rv;
    bcm_oam_endpoint_info_t mep_acc_info;
    int mep_id = 0;
    int mep_type;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;
    int tx_port;
    int counter_base;
    int counter_if;
    int mdl = 7;
    int is_primary;
    int nof_objects = 2;

    counter_if = lm_counter_if_mpls;
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_mpls_pri);
    BCM_IF_ERROR_RETURN(rv);
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_mpls_bp);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Adding two acc MEPs, one for MPLS-TP backup, another for MPLS-TP primary
     */
    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        mep_type = bcmOAMEndpointTypeBHHMPLS;
        gport = BCM_GPORT_INVALID;
        mpls_out_gport = is_primary ? cint_vpls_basic_info.mpls_tunnel_id_primary : cint_vpls_basic_info.mpls_tunnel_id;
        BCM_GPORT_TUNNEL_ID_SET(mpls_out_gport, mpls_out_gport);
        mpls_tunnel_gport = is_primary ? cint_vpls_basic_info.mpls_tunnel_id_primary : cint_vpls_basic_info.mpls_tunnel_id;
        tx_port = lm_pwe_port;
        counter_if = lm_counter_if_mpls;
        counter_base = is_primary ? lm_counter_base_id_mpls_pri : lm_counter_base_id_mpls_bp;
        mep_id = is_mep_with_id ? (is_primary ? mep_acc_mpls_pri_info.id : mep_acc_mpls_bp_info.id) : 0;
        rv = oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info_short_ma.id, counter_base, counter_if,
                 gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
                 is_mep_with_id, mep_id, mdl, is_dual_ended_lm, is_primary);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
            return rv;
        }

        if (is_primary)
        {
            sal_memcpy(&mep_acc_mpls_pri_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for MPLS-TP primary with id %d\n", mep_acc_info.id);
        }
        else
        {
            sal_memcpy(&mep_acc_mpls_bp_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for MPLS-TP backup with id %d\n", mep_acc_info.id);
        }
    }

    return rv;
}


/*
 * Create 2 PWE MEPs for HLM in egress with PWE protection.
 */
int
oam_run_hlm_egress_mpls_section_mep_create (
    int unit,
    int is_mep_with_id)
{
    bcm_error_t rv;
    bcm_oam_endpoint_info_t mep_acc_info;
    int mep_id = 0;
    int mep_type;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;
    int tx_port;
    int counter_base;
    int counter_if;
    int mdl = 7;
    int is_primary;
    int nof_objects = 2;

    counter_if = lm_counter_if_section;
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_section_pri);
    BCM_IF_ERROR_RETURN(rv);
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_section_bp);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Adding two acc MEPs, one for MPLS-SECTION backup, another for MPLS-SECTION primary
     */
    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        mep_type = bcmOAMEndpointTypeBhhSection;
        gport = BCM_GPORT_INVALID;
        mpls_out_gport = is_primary ? cint_vpls_basic_info.core_arp_id_primary : cint_vpls_basic_info.core_arp_id;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(mpls_out_gport, mpls_out_gport);
        mpls_tunnel_gport = is_primary ? cint_vpls_basic_info.core_arp_id_primary : cint_vpls_basic_info.core_arp_id;
        tx_port = lm_pwe_port;
        counter_if = lm_counter_if_section;
        counter_base = is_primary ? lm_counter_base_id_section_pri : lm_counter_base_id_section_bp;
        mep_id = is_mep_with_id ? (is_primary ? mep_acc_section_pri_info.id : mep_acc_section_bp_info.id) : 0;
        rv = oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info_short_ma.id, counter_base, counter_if,
                 gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
                 is_mep_with_id, mep_id, mdl, is_dual_ended_lm, is_primary);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
            return rv;
        }

        if (is_primary)
        {
            sal_memcpy(&mep_acc_section_pri_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for MPLS-SECTION primary with id %d\n", mep_acc_info.id);
        }
        else
        {
            sal_memcpy(&mep_acc_section_bp_info, &mep_acc_info, sizeof(mep_acc_info));
            printf("created acc MEP for MPLS-SECTION backup with id %d\n", mep_acc_info.id);
        }
    }

    return rv;
}

/*
 * 2. Example of HLM in egress with PWE protection
 */
int
oam_run_hlm_egress_with_pwe_protection(
    int unit,
    int ac_port,
    int pwe_port)
{
    bcm_error_t rv;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
    int raw_lif_action_profile_id = 1;
    uint32 flags_get = 0;
    int egr_profile_id = 0;
    bcm_oam_profile_type_t profile_type = bcmOAMProfileEgressLIF;

    /*
     * Setup VPLS PWE protection model
     */
    pwe_protection_with_one_fec = 1;

    rv = vpls_mp_basic_main(unit, ac_port, pwe_port);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in vpls_mp_basic_main\n", rv);
        return rv;
    }
    lm_pwe_port = pwe_port;

    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }
    printf("Created group short name format\n");

    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    rv = bcm_oam_mpls_tp_channel_type_tx_set(unit, bcmOamMplsTpChannelPweonoam, 0x8902);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_mpls_tp_channel_type_tx_set.\n", rv);
        return rv;
    }

    /*
     * Adding two acc MEPs, one for PWE backup, another for PWE primary
     */
    rv = oam_run_hlm_egress_pwe_mep_create(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in oam_run_hlm_egress_pwe_mep_create\n", rv);
        return rv;
    }

    /*
     * Adding two acc MEPs, one for MPLS-TP backup, another for MPLS-TP primary
     */
    rv = oam_run_hlm_egress_mpls_tp_mep_create(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in oam_run_hlm_egress_mpls_tp_mep_create\n", rv);
        return rv;
    }

    if (is_hlm_egress_with_section) {
        /*
         * Adding two acc MEPs, one for MPLS-SECTION backup, another for MPLS-SECTION primary
         */
        rv = oam_run_hlm_egress_mpls_section_mep_create(unit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error rv(%d), in oam_run_hlm_egress_mpls_section_mep_create\n", rv);
            return rv;
        }
    }

    /*
     * Speculate CW protocol after the MPLS stack
     * This is needed to identify y1731 oam over pwe in egress parser
     */
    rv = dnx_oam_identify_y1731_oam_over_pwe_egress(unit);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in dnx_oam_identify_y1731_oam_over_pwe_egress\n", rv);
        return rv;
    }

    /*
     * Update counter in egress profile
     *
     * For egress counter for mpls oam injected packet,
     * 1) Counter should always be enabled in non acc egress profile by calling dnx_oam_egress_with_update_counter_in_egr_profile
     * 2) Users can control whether the packet should be counted, by setting reserved egress acc profile.
     *    Please refer to appl_dnx_oam_hlm_egress_with_update_counter_in_egr_acc_profile
 */
    rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, profile_type, &flags_get, &egr_profile_id);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_profile_id_get_by_type\n", rv);
        return rv;
    }

    rv = dnx_oam_egress_with_update_counter_in_egr_profile(unit, $egr_profile_id, bcmOamOpcodeLMM, 1);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in dnx_oam_egress_with_update_counter_in_egr_profile\n", rv);
        return rv;
    }

    rv = dnx_oam_egress_with_update_counter_in_egr_profile(unit, $egr_profile_id, bcmOamOpcodeDMM, 1);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in dnx_oam_egress_with_update_counter_in_egr_profile\n", rv);
        return rv;
    }

    rv = dnx_oam_egress_with_update_counter_in_egr_profile(unit, $egr_profile_id, bcmOamOpcodeCCM, 1);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in dnx_oam_egress_with_update_counter_in_egr_profile\n", rv);
        return rv;
    }

    /*
     * Adding l2 addr
     */
    rv = oam_run_hlm_egress_pwe_l2_addr_add(unit);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in oam_run_hlm_egress_pwe_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

