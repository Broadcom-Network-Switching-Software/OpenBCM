/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_oam_y1731_over_pwe_protection.c
 *
 * Purpose: basic example for oam over pwe protection.
 *
 * Examples:
 *   - 1. Example of oamp injection with PWE protection
 *   - 2. Example of Hierarchial LM (HLM) in egress with PWE protection
 *
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
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/field/cint_field_utils.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/oam/cint_oam_action.c
  cint ../../../../src/examples/dnx/vpls/cint_dnx_vpls_mp_basic.c
  cint ../../../../src/examples/dnx/oam/cint_dnx_oam_y1731_over_pwe_protection.c
  cint
  cint_vpls_pwe_protection_info.cw_present=1;
 *
 * Step 1. Load cint_vpls_pwe_protection_info to setup pwe protection model, and
 *         create oam group, Lif profile and two ACC MEPs. One for PWE primary, another for PWE backup
  oam_run_with_pwe_protection(0,200,201);
  exit;
 *
 * Step 2. Test without ignoring protection state HW capability, and switch to backup pwe, then primary pwe
  cint
  vpls_pwe_protection_basic_set_failover_ignore(0,0);
  exit;
 *
 * Step 2.1 Switch to backup pwe
  cint
  vpls_pwe_protection_basic_set_failover(0,0);
  exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Step 2.2 Switch to primary pwe
  cint
  vpls_pwe_protection_basic_set_failover(0,1);
  exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Step 3. Test with ignoring protection state HW capability, if device support
 *     Note: currently, J2C, Q2A and above support
  cint
  vpls_pwe_protection_basic_set_failover_ignore(0,1);
  exit;
 *
 * Step 3.1 Switch to backup pwe
  cint
  vpls_pwe_protection_basic_set_failover(0,0);
  exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Step 3.2 Switch to primary pwe
  cint
  vpls_pwe_protection_basic_set_failover(0,1);
  exit;
 *
 * Injected CCM packets from endpoint for PWE backup, received packets (with LSP label for backup) should be:
 *     Data: 0x00000000cd1d000c000200018100f01e884700d05e0000bb830a10008902e0010346000000001235010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * Injected CCM packets from endpoint for PWE primary, received packets (with LSP label for primary) should be:
 *     Data: 0x00000000cd1d000c000200018100f01f884700d07e0000bb830a10008902e0010346000000001234010302abcd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 * Test Scenario - end
 *
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
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/field/cint_field_utils.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
  cint ../../../../src/examples/dnx/oam/cint_oam_action.c
  cint ../../../../src/examples/dnx/vpls/cint_dnx_vpls_mp_basic.c
  cint ../../../../src/examples/dnx/oam/cint_dnx_oam_y1731_over_pwe_protection.c
  cint
  cint_vpls_pwe_protection_info.cw_present=1;
  is_hlm_egress_with_section=1;
 *
 * Step 1. Load cint_vpls_pwe_protection_info to setup pwe protection model, and
 *         create oam group, Lif profile and two ACC MEPs. One for PWE primary, another for PWE backup
  oam_run_hlm_egress_with_pwe_protection(0,200,201);
  exit;
 * Test Scenario - end
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
 *
 */

bcm_oam_group_info_t group_info;

bcm_oam_endpoint_info_t mep_acc_pwe_bp_info;
bcm_oam_endpoint_info_t mep_acc_pwe_pri_info;
bcm_oam_endpoint_info_t mep_acc_mpls_bp_info;
bcm_oam_endpoint_info_t mep_acc_mpls_pri_info;
bcm_oam_endpoint_info_t mep_acc_section_bp_info;
bcm_oam_endpoint_info_t mep_acc_section_pri_info;

/* JR3 only */
bcm_oam_endpoint_info_t mep_acc_lm_pwe_bp_info;
bcm_oam_endpoint_info_t mep_acc_lm_pwe_pri_info;
bcm_oam_endpoint_info_t mep_acc_lm_mpls_bp_info;
bcm_oam_endpoint_info_t mep_acc_lm_mpls_pri_info;
bcm_oam_endpoint_info_t mep_acc_lm_section_bp_info;
bcm_oam_endpoint_info_t mep_acc_lm_section_pri_info;
bcm_oam_endpoint_info_t mep_acc_dm_pwe_bp_info;
bcm_oam_endpoint_info_t mep_acc_dm_pwe_pri_info;
bcm_oam_endpoint_info_t mep_acc_dm_mpls_bp_info;
bcm_oam_endpoint_info_t mep_acc_dm_mpls_pri_info;
bcm_oam_endpoint_info_t mep_acc_dm_section_bp_info;
bcm_oam_endpoint_info_t mep_acc_dm_section_pri_info;

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


/*
 * Failover Set
 */
int
vpls_pwe_protection_basic_set_failover (
    int unit,
    int is_primary)
{
    char error_msg[200]={0,};
    bcm_failover_t failover_id, egress_failover_id;
    int egress_enable;

    failover_id = cint_vpls_basic_info.pwe_failover_id_fec;
    egress_failover_id = cint_vpls_basic_info.pwe_failover_id_outlif;

    snprintf(error_msg, sizeof(error_msg), "failover_id:  0x%08x", failover_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_failover_set(unit, failover_id, is_primary), error_msg);

    if (*dnxc_data_get(unit, "multicast", "params", "mc_pruning_actions_not_supported", NULL))
    {
        snprintf(error_msg, sizeof(error_msg), "failover_id:  0x%08x", egress_failover_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_failover_set(unit, egress_failover_id, is_primary), error_msg);
    }

    return BCM_E_NONE;
}

int
vpls_pwe_protection_basic_set_failover_ignore (
    int unit,
    int is_ignore)
{
    char error_msg[200]={0,};
    int protection_state_ignore_support = *(dnxc_data_get(unit, "device", "general", "protection_state_ignore", NULL));

    if (!protection_state_ignore_support)
    {
        printf("Ignore Failover state is not supported by this device\n", rv);
        return BCM_E_UNAVAIL;
    }

    snprintf(error_msg, sizeof(error_msg), "in bcmSwitchControlOamBfdFailoverStateIgnore is_ignore: %d", is_ignore);
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchControlOamBfdFailoverStateIgnore, is_ignore), error_msg);

    return BCM_E_NONE;
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
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, flags, gport, ingress_profile_id, egress_profile_id), "");
    }

    if (mpls_out_gport != BCM_GPORT_INVALID)
    {
        /** Create egress profile - profile is used, but no actions are performed. */
        egress_profile_id = 1;
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id), "(egress)");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, flags, mpls_out_gport, BCM_OAM_PROFILE_INVALID, egress_profile_id), "");
    }

    return BCM_E_NONE;
}

/**
 * DNX ONLY.
 * Configure ingress and egress profiles for MPLS Section LIF.
 * Use cint_oam_action.c to create action profiles for OAM
 * section.
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
    int lm_type;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile_id = 1, acc_ingress_profile = 1, egress_profile_id = BCM_OAM_PROFILE_INVALID;

    /** Set lm type */
    lm_type = is_dual_ended_lm ? 1 : 0;

    /** Create ingress oam section profile */
    BCM_IF_ERROR_RETURN_MSG(oam_section_action_profile_create(unit,lm_type,&ingress_profile_id,&acc_ingress_profile), "");

    mep_info->acc_profile_id = acc_ingress_profile;
    if (gport != BCM_GPORT_INVALID)
    {
        flags = 0;
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, flags, gport, ingress_profile_id, egress_profile_id), "");
    }

    if (mpls_out_gport != BCM_GPORT_INVALID)
    {
        /** Create egress profile - profile is used, but no actions are performed. */
        egress_profile_id = 1;
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id), "(egress)");

        BCM_IF_ERROR_RETURN_MSG(bcm_oam_lif_profile_set(unit, flags, mpls_out_gport, BCM_OAM_PROFILE_INVALID, egress_profile_id), "");
    }

    return BCM_E_NONE;
}

/**
 * DNX ONLY.
 * Create MEP for PWE or MPLS-TP or MPLS-Section, based on mep_type
 *
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
    bcm_oam_loss_t loss_info;
    bcm_oam_delay_t delay_info;
    bcm_oam_endpoint_info_t lm_mep_info;
    bcm_oam_endpoint_info_t dm_mep_info;
    int loss_id = 0;
    int device_type_v2 = *(dnxc_data_get(unit,  "oam", "oamp", "oamp_v2", NULL));

    /* 1. Create OAM MPLS/PWE MEP */
    bcm_oam_endpoint_info_t_init(mep_acc_info);
    mep_acc_info->type = mep_type;
    mep_acc_info->endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;

    /*RX*/
    mep_acc_info->group = group;
    if (device_type_v2 && is_dual_ended_lm) {
        mep_acc_info->opcode_flags |= BCM_OAM_OPCODE_DUAL_ENDED_LM;
    } else {
        mep_acc_info->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    }
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
        BCM_IF_ERROR_RETURN_MSG(config_oam_section_profile(unit, gport, out_gport, is_dual_ended_lm, mep_acc_info), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(config_oam_mpls_or_pwe_profile(unit, gport, out_gport, is_dual_ended_lm, mep_acc_info), "");
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, mep_acc_info), "");

    /* 2. Add loss on MEP */
    if (device_type_v2 && !is_dual_ended_lm) {
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_get(unit, mep_acc_info->id, &lm_mep_info), "");
        lm_mep_info.name = 0;
        lm_mep_info.opcode_flags = BCM_OAM_OPCODE_LMM_IN_HW;
        printf("create lm endpoint\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, lm_mep_info), "");
        if (is_primary) {
            if (bcmOAMEndpointTypeBHHMPLS == mep_type) {
                sal_memcpy(&mep_acc_lm_mpls_pri_info, &lm_mep_info, sizeof(lm_mep_info));
            } else if (bcmOAMEndpointTypeBHHPwe == mep_type) {
                sal_memcpy(&mep_acc_lm_pwe_pri_info, &lm_mep_info, sizeof(lm_mep_info));
            } else {
                sal_memcpy(&mep_acc_lm_section_pri_info, &lm_mep_info, sizeof(lm_mep_info));
            }
        } else {
            if (bcmOAMEndpointTypeBHHMPLS == mep_type) {
                sal_memcpy(&mep_acc_lm_mpls_bp_info, &lm_mep_info, sizeof(lm_mep_info));
            } else if (bcmOAMEndpointTypeBHHPwe == mep_type) {
                sal_memcpy(&mep_acc_lm_pwe_bp_info, &lm_mep_info, sizeof(lm_mep_info));
            } else {
                sal_memcpy(&mep_acc_lm_section_bp_info, &lm_mep_info, sizeof(lm_mep_info));
            }
        }
    }
    if (!device_type_v2) {
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
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_loss_add(unit, &loss_info), "");
    }

    /* 3. Add delay on MEP */
    if (device_type_v2) {
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_get(unit, mep_acc_info->id, &dm_mep_info), "");
        dm_mep_info.name = 0;
        dm_mep_info.tlvs[0].tlv_type = bcmOamTlvTypeData;
        dm_mep_info.tlvs[0].four_byte_repeatable_pattern = 0;
        dm_mep_info.tlvs[0].tlv_length = 8;
        dm_mep_info.opcode_flags = BCM_OAM_OPCODE_DMM_IN_HW;
        printf("create dm endpoint\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, dm_mep_info), "");
        if (is_primary) {
            if (bcmOAMEndpointTypeBHHMPLS == mep_type) {
                sal_memcpy(&mep_acc_dm_mpls_pri_info, &dm_mep_info, sizeof(dm_mep_info));
            } else if (bcmOAMEndpointTypeBHHPwe == mep_type) {
                sal_memcpy(&mep_acc_dm_pwe_pri_info, &dm_mep_info, sizeof(dm_mep_info));
            } else {
                sal_memcpy(&mep_acc_dm_section_pri_info, &dm_mep_info, sizeof(dm_mep_info));
            }
        } else {
            if (bcmOAMEndpointTypeBHHMPLS == mep_type) {
                sal_memcpy(&mep_acc_dm_mpls_bp_info, &dm_mep_info, sizeof(dm_mep_info));
            } else if (bcmOAMEndpointTypeBHHPwe == mep_type) {
                sal_memcpy(&mep_acc_dm_pwe_bp_info, &dm_mep_info, sizeof(dm_mep_info));
            } else {
                sal_memcpy(&mep_acc_dm_section_bp_info, &dm_mep_info, sizeof(dm_mep_info));
            }
        }
    } else {
        bcm_oam_delay_t_init(&delay_info);
        delay_info.id = mep_acc_info->id;
        delay_info.flags = 0;
        delay_info.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
        delay_info.period = 0;
        printf("bcm_oam_delay_add delay_info\n");
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_delay_add(unit, &delay_info), "");
    }

    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(vpls_mp_basic_main(unit, ac_port, pwe_port), "");

    bcm_oam_group_info_t_init(&group_info);
    sal_memcpy(group_info.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &group_info), "");
    printf("Created group short name format\n");

    BCM_IF_ERROR_RETURN_MSG(oam_set_of_action_profiles_create(unit), "");

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
        BCM_IF_ERROR_RETURN_MSG(config_oam_mpls_or_pwe_profile(unit, gport, mpls_out_gport, 0, &mep_acc_info), "");

        /*RX*/
        mep_acc_info.group = group_info.id;
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
        BCM_IF_ERROR_RETURN_MSG(bcm_oam_endpoint_create(unit, &mep_acc_info), "");

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

    return BCM_E_NONE;
}

/*
 * Add l2 addr for data traffic to pwe.
 */
int
oam_run_hlm_egress_pwe_l2_addr_add (
    int unit)
{
    bcm_l2_addr_t l2addr;
    int is_primary = 0;
    int nof_objects = 2;

    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        bcm_l2_addr_t_init(&l2addr, (is_primary?pwe_mac_addr_pri:pwe_mac_addr_bp), cint_vpls_basic_info.vpn);
        l2addr.flags    = BCM_L2_STATIC;
        l2addr.port     = lm_pwe_port;
        l2addr.encap_id = is_primary ? cint_vpls_basic_info.encap_id_primary : cint_vpls_basic_info.encap_id;

        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), "");
    }

    return BCM_E_NONE;
}

/*
 * Create 2 PWE MEPs for HLM in egress with PWE protection.
 */
int
oam_run_hlm_egress_pwe_mep_create (
    int unit,
    int is_mep_with_id)
{
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
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_pwe_pri), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_pwe_bp), "");

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
        BCM_IF_ERROR_RETURN_MSG(oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info.id, counter_base, counter_if,
                 gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
                 is_mep_with_id, mep_id, mdl, is_dual_ended_lm, is_primary), "");

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

    return BCM_E_NONE;
}

/*
 * Create 2 PWE MEPs for HLM in egress with PWE protection.
 */
int
oam_run_hlm_egress_mpls_tp_mep_create (
    int unit,
    int is_mep_with_id)
{
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
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_mpls_pri), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_mpls_bp), "");

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
        BCM_IF_ERROR_RETURN_MSG(oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info.id, counter_base, counter_if,
                 gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
                 is_mep_with_id, mep_id, mdl, is_dual_ended_lm, is_primary), "");

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

    return BCM_E_NONE;
}


/*
 * Create 2 PWE MEPs for HLM in egress with PWE protection.
 */
int
oam_run_hlm_egress_mpls_section_mep_create (
    int unit,
    int is_mep_with_id)
{
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
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_section_pri), "");
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_section_bp), "");

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
        BCM_IF_ERROR_RETURN_MSG(oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info.id, counter_base, counter_if,
                 gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
                 is_mep_with_id, mep_id, mdl, is_dual_ended_lm, is_primary), "");

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

    return BCM_E_NONE;
}

/*
 * 2. Example of HLM in egress with PWE protection
 *
 */
int
oam_run_hlm_egress_with_pwe_protection(
    int unit,
    int ac_port,
    int pwe_port)
{
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
    uint8 long_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 32, 13, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0xa, 0xb, 0xc, 0xd};
    int raw_lif_action_profile_id = 1;
    uint32 flags_get = 0;
    int egr_profile_id = 0;
    bcm_oam_profile_type_t profile_type = bcmOAMProfileEgressLIF;
    int device_type_v2 = *(dnxc_data_get(unit,  "oam", "oamp", "oamp_v2", NULL));
    int i;

    /*
     * Setup VPLS PWE protection model
     */
    pwe_protection_with_one_fec = 1;

    BCM_IF_ERROR_RETURN_MSG(vpls_mp_basic_main(unit, ac_port, pwe_port), "");
    lm_pwe_port = pwe_port;

    bcm_oam_group_info_t_init(&group_info);
    if (device_type_v2)
    {
        sal_memcpy(group_info.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
    }
    else
    {
        sal_memcpy(group_info.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_group_create(unit, &group_info), "");
    printf("Created group\n");

    BCM_IF_ERROR_RETURN_MSG(oam_set_of_action_profiles_create(unit), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_oam_mpls_tp_channel_type_tx_set(unit, bcmOamMplsTpChannelPweonoam, 0x8902), "");

    /*
     * Adding two acc MEPs, one for PWE backup, another for PWE primary
     */
    BCM_IF_ERROR_RETURN_MSG(oam_run_hlm_egress_pwe_mep_create(unit, 0), "");

    /*
     * Adding two acc MEPs, one for MPLS-TP backup, another for MPLS-TP primary
     */
    BCM_IF_ERROR_RETURN_MSG(oam_run_hlm_egress_mpls_tp_mep_create(unit, 0), "");

    if (is_hlm_egress_with_section) {
        /*
         * Adding two acc MEPs, one for MPLS-SECTION backup, another for MPLS-SECTION primary
         */
        BCM_IF_ERROR_RETURN_MSG(oam_run_hlm_egress_mpls_section_mep_create(unit, 0), "");
    }

    /*
     * Speculate CW protocol after the MPLS stack
     * This is needed to identify y1731 oam over pwe in egress parser
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_oam_identify_y1731_oam_over_pwe_egress(unit), "");

    /*
     * Update counter in egress profile
     *
     * For egress counter for mpls oam injected packet,
     * 1) Counter should always be enabled in non acc egress profile by calling dnx_oam_egress_with_update_counter_in_egr_profile
     * 2) Users can control whether the packet should be counted, by setting reserved egress acc profile.
     *    Please refer to appl_dnx_oam_hlm_egress_with_update_counter_in_egr_acc_profile
     *
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, profile_type, &flags_get, &egr_profile_id), "");

    BCM_IF_ERROR_RETURN_MSG(dnx_oam_egress_with_update_counter_in_egr_profile(unit, $egr_profile_id, bcmOamOpcodeLMM, 1), "");

    BCM_IF_ERROR_RETURN_MSG(dnx_oam_egress_with_update_counter_in_egr_profile(unit, $egr_profile_id, bcmOamOpcodeDMM, 1), "");

    BCM_IF_ERROR_RETURN_MSG(dnx_oam_egress_with_update_counter_in_egr_profile(unit, $egr_profile_id, bcmOamOpcodeCCM, 1), "");

    /*
     * Adding l2 addr
     */
    BCM_IF_ERROR_RETURN_MSG(oam_run_hlm_egress_pwe_l2_addr_add(unit), "");

    return BCM_E_NONE;
}

