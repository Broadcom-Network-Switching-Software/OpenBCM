/*
 * $Id: cint_oam_action.c
n.c This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
n.c 
n.c Copyright 2007-2020 Broadcom Inc. All rights reserved.File: cint_oam_slm.c Purpose: Examples for JR OAM SLM. 
 * This file includes examples of setting different types of OAM action profiles: 
 *  - Single Ended
 *  - Double Ended
 *  - LMM
 *
 *
 * Main function: oam_set_of_action_profiles_create : Configures set of different action profile types:
 *     - for single ended, dual ended,
 *     - for ETH,MPLS,PWE 
 * Input: is_ingress: 1 for ingress profile set, 0 for egress
 *  
 * As a result the LIF-action and Accelerated-MEP-action tables will be configured as follows: 
 *  
 * ================================================================================= 
 *    Profile No  ||         Profile Name         || LIF TYPE  | LM TYPE 
 * ================================================================================= 
 *       2        || oam_profile_eth_single_ended  ||   ETH    |   Single Ended
 *       3        || oam_profile_eth_dual_ended    ||   ETH    |   Dual Ended
 *       4        || oam_profile_eth_none          ||   ETH    |   NONE
 *       5        || oam_profile_mpls_single_ended ||   MPLS   |   Single Ended
 *       6        || oam_profile_mpls_dual_ended   ||   MPLS   |   Dual Ended
 *       7        || oam_profile_mpls_none         ||   MPLS   |   NONE
 *       8        || oam_profile_pwe_single_ended  ||   PWE    |   Single Ended
 *       9        || oam_profile_pwe_dual_ended    ||   PWE    |   Dual Ended
 *      10        || oam_profile_pwe_none          ||   PWE    |   NONE
 *
 *
 *         Ingress/Egress Accelerated
 * ===============================================================================
 * Acc Profile No ||           Profile Name            || LIF TYPE | LM TYPE 
 * ===============================================================================
 *       2        || oam_acc_profile_eth_single_ended  ||  ETH    |   Single Ended
 *       3        || oam_acc_profile_eth_dual_ended    ||  ETH    |   Dual Ended
 *       4        || oam_acc_profile_eth_none          ||  ETH    |   NONE
 *       5        || oam_acc_profile_mpls_single_ended ||  MPLS   |   Single Ended
 *       6        || oam_acc_profile_mpls_dual_ended   ||  MPLS   |   Dual Ended
 *       7        || oam_acc_profile_mpls_none         ||  MPLS   |   NONE
 *       8        || oam_acc_profile_pwe_single_ended  ||  PWE    |   Single Ended
 *       9        || oam_acc_profile_pwe_dual_ended    ||  PWE    |   Dual Ended
 *      10        || oam_acc_profile_pwe_none          ||  PWE    |   NONE
 *   
 *
 * Once oam_set_of_action_profiles_create is run, binding a specific profile to a lif is required.
 * Different profile out of the profile set may be binded to each lif.
 * For Accelerated MEPs, both acc_profile and non_acc_profile should be set. In this case non_acc_profile 
 * only configures the counting properties of the LIF.
 *
 * Section MEPs will also use PWE profiles
 *
 * Examples for endpoints create can be found in cint_oam_basic.c
 */

/**
 * LM_TYPE Definitions 
 */
static const int LM_TYPE_SINGLE_ENDED = 0;
static const int LM_TYPE_DUAL_ENDED = 1;
static const int LM_TYPE_NONE = 2;

/**
 * LIF_TYPE Definitions 
 */
static const int MEP_TYPE_ETH = 0;
static const int MEP_TYPE_MPLS = 1;
static const int MEP_TYPE_PWE = 2;

/**
 * Table for all supported opcodes.
 */
bcm_oam_opcode_t supported_opcodes[] = {
    bcmOamOpcodeCCM,
    bcmOamOpcodeLBR,
    bcmOamOpcodeLBM,
    bcmOamOpcodeLTR,
    bcmOamOpcodeLTM,
    bcmOamOpcodeAIS,
    bcmOamOpcodeLCK,
    bcmOamOpcodeLinearAPS,
    bcmOamOpcodeLMR,
    bcmOamOpcodeLMM,
    bcmOamOpcode1DM,
    bcmOamOpcodeDMR,
    bcmOamOpcodeDMM,
    bcmOamOpcodeSLR,
    bcmOamOpcodeSLM
};

/**
 * Enable egress pcp counting
 */
int is_pcp_counting_enabled = 0;

/*
 * Destination Gports Struct
 */
struct oam_destinations_s
{
    bcm_gport_t active_down_dest_gport;
    bcm_gport_t active_up_dest_gport;
    bcm_gport_t snoop_dest_gport;
    bcm_gport_t recycle_dest_gport;
    bcm_gport_t cpu_dest_gport;
    bcm_gport_t level_dest_gport_ingress;
    bcm_gport_t level_dest_gport_egress;
    bcm_gport_t oamp_eth_dest_gport;
    bcm_gport_t oamp_eth_performance_dest_gport;
    bcm_gport_t oamp_mpls_dest_gport;
    bcm_gport_t oamp_mpls_performance_dest_gport;
    bcm_gport_t oamp_pwe_dest_gport;
    bcm_gport_t oamp_pwe_performance_dest_gport;
    bcm_gport_t passive_dest_gport_ingress;
    bcm_gport_t passive_dest_gport_egress;
};

/*
 * Structure of acc-lif-profiles
 */
struct oam_lif_profiles_s
{
   bcm_oam_profile_t oam_profile_eth_single_ended;
   bcm_oam_profile_t oam_profile_eth_dual_ended;
   bcm_oam_profile_t oam_profile_eth_none;
   bcm_oam_profile_t oam_profile_mpls_single_ended;
   bcm_oam_profile_t oam_profile_mpls_dual_ended;
   bcm_oam_profile_t oam_profile_mpls_none;
   bcm_oam_profile_t oam_profile_pwe_single_ended;
   bcm_oam_profile_t oam_profile_pwe_dual_ended;
   bcm_oam_profile_t oam_profile_pwe_none;
};

/*
 * Structure of acc-lif-profiles
 */
struct oam_acc_lif_profiles_s
{
   bcm_oam_profile_t oam_acc_profile_eth_single_ended;
   bcm_oam_profile_t oam_acc_profile_eth_dual_ended;
   bcm_oam_profile_t oam_acc_profile_eth_none;
   bcm_oam_profile_t oam_acc_profile_mpls_single_ended;
   bcm_oam_profile_t oam_acc_profile_mpls_dual_ended;
   bcm_oam_profile_t oam_acc_profile_mpls_none;
   bcm_oam_profile_t oam_acc_profile_pwe_single_ended;
   bcm_oam_profile_t oam_acc_profile_pwe_dual_ended;
   bcm_oam_profile_t oam_acc_profile_pwe_none;
};

/* 
 * lif profiles
 */
oam_lif_profiles_s oam_lif_profiles;
oam_acc_lif_profiles_s oam_acc_lif_profiles;

/*
 * Table of lif-profiles
 */
bcm_oam_profile_t * oam_lif_profiles_t[]=
{
   &oam_lif_profiles.oam_profile_eth_single_ended,
   &oam_lif_profiles.oam_profile_eth_dual_ended,
   &oam_lif_profiles.oam_profile_eth_none,
   &oam_lif_profiles.oam_profile_mpls_single_ended,
   &oam_lif_profiles.oam_profile_mpls_dual_ended,
   &oam_lif_profiles.oam_profile_mpls_none,
   &oam_lif_profiles.oam_profile_pwe_single_ended,
   &oam_lif_profiles.oam_profile_pwe_dual_ended,
   &oam_lif_profiles.oam_profile_pwe_none
};

/*
 * OAM Section Profiles
 */
bcm_oam_profile_t oam_section_acc_profile;
bcm_oam_profile_t oam_section_profile;

/*
 * Table of acc-lif-profiles
 */
bcm_oam_profile_t * oam_acc_lif_profiles_t[]=
{
   &oam_acc_lif_profiles.oam_acc_profile_eth_single_ended,
   &oam_acc_lif_profiles.oam_acc_profile_eth_dual_ended,
   &oam_acc_lif_profiles.oam_acc_profile_eth_none,
   &oam_acc_lif_profiles.oam_acc_profile_mpls_single_ended,
   &oam_acc_lif_profiles.oam_acc_profile_mpls_dual_ended,
   &oam_acc_lif_profiles.oam_acc_profile_mpls_none,
   &oam_acc_lif_profiles.oam_acc_profile_pwe_single_ended,
   &oam_acc_lif_profiles.oam_acc_profile_pwe_dual_ended,
   &oam_acc_lif_profiles.oam_acc_profile_pwe_none
};

/* indication that Hierarchical LM disable */
uint32 hierarchical_lm_disable = 0;
/*
 * Example of lm action configuration for a profile_id 
 * LM or SLM action may be selected 
 */
int
oam_lm_action(
    int unit,
    uint8 is_ingress,
    bcm_oam_profile_t profile_id,
    int is_dual_ended)
{
    bcm_error_t rv;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    int endpoint_type;
    int dest_mac_type;
    bcm_gport_t level_dest_gport;
    bcm_gport_t cpu_dest_gport;
    bcm_gport_t recycle_dest_gport;
    bcm_gport_t snoop_dest_gport;
    uint32 flags;
    int skip_config;
    int opcode_index;
    int nof_supported_opcodes;
    int nof_endpoint_types;
    int nof_mac_types;
    bcm_rx_trap_config_t trap_config;
    int trap_id, trap_id_level;
    bcm_gport_t active_dest_gport;
    bcm_gport_t passive_dest_gport;
    bcm_gport_t cpu_port;
    rv = BCM_E_NONE;
    flags = 0;

    active_dest_gport = BCM_GPORT_INVALID;
    passive_dest_gport = BCM_GPORT_INVALID;

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapBfdOamDownMEP, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(cpu_dest_gport, trap_id, 7, 0);

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamLevel, &trap_id_level);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(level_dest_gport, trap_id_level, 7, 0);

    recycle_dest_gport = active_dest_gport;
    snoop_dest_gport = active_dest_gport;

    /*
     * Loss action is set in two phases: 
     * 1. Set loss type: Configure action for lm or dual ended 
     * 2. Set counting and trap code configuration by configuring all 
     *    the action table with all possible keys  
     */

    /*
     * Prepare action_key and action_result 
     *  
     * The key should be 0 when configuring the lm_type 
     */
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = 0;

    action_result.flags = 0;
    action_result.meter_enable = 0;
    action_result.destination = BCM_GPORT_INVALID;

    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
        return rv;
    }

    nof_supported_opcodes = 15;
    nof_mac_types = 3;
    nof_endpoint_types = 4;

    /*
     * Configure all mac types
     */
    for (dest_mac_type = 0; dest_mac_type < nof_mac_types; dest_mac_type++)
    {
        action_key.dest_mac_type = dest_mac_type;
        /*
         * Configure all supported OAM opcodes
         */
        for (opcode_index = 0; opcode_index < nof_supported_opcodes; opcode_index++)
        {
            action_key.opcode = supported_opcodes[opcode_index];

            /*
             * Configure all endpoint match types
             */
            for (endpoint_type = 0; endpoint_type < nof_endpoint_types; endpoint_type++)
            {
                /*
                 * Don't count OAM packets 
                 * (Except for CCM packets in LM-MEPs)
                 */
                action_result.counter_increment = 0;
                skip_config = 0;
                action_key.endpoint_type = endpoint_type;
                /*
                 *  Configure endpoint_type dependent variables 
                 *  Specific cases that are configured separately,
                 *  are skipped.
 */
                switch (endpoint_type)
                {
                    case bcmOAMMatchTypeMEP:
                    {
                        /*
                         * Set action for packets that match the MEP
                         */
                        if ((is_dual_ended == 0) && (action_key.opcode == bcmOamOpcodeCCM))
                        {
                            /*
                             * For LM action count only OAM-CCM packets 
                             */
                            action_result.counter_increment = 1;
                        }

                        /*
                         * Send matching packets to active_dest_port 
                         *  
                         *  Destination port table for MEP:
                         *  
                         *  Opcode |   UniCast   Multicast   UnknownUniCast
                         *  -----------------------------------------------
                         *  XXM    |    active     active      error
                         *  LBM    |    recycle    active      error
                         *  CCM    |    active     active      error
                         *  Other  |    active     error       error
                         */
                        if (dest_mac_type == bcmOAMDestMacTypeUknownUcast)
                        {
                            action_result.destination = level_dest_gport;
                        }
                        else if (dest_mac_type == bcmOAMDestMacTypeMcast)
                        {
                            if ((action_key.opcode == bcmOamOpcodeCCM) || (action_key.opcode == bcmOamOpcodeLTM) ||
                                (action_key.opcode == bcmOamOpcodeLBM) || (action_key.opcode == bcmOamOpcodeLMM) ||
                                (action_key.opcode == bcmOamOpcodeAIS) || (action_key.opcode == bcmOamOpcodeDMM) ||
                                (action_key.opcode == bcmOamOpcodeLinearAPS) || (action_key.opcode == bcmOamOpcodeSLM))
                            {
                                action_result.destination = cpu_dest_gport;
                            }
                            else
                            {
                                action_result.destination = level_dest_gport;
                            }
                        }
                        else
                        {
                            /*
                             * Unicast
                             */
                            if (action_key.opcode == bcmOamOpcodeLBM)
                            {
                                action_result.destination = recycle_dest_gport;
                            }
                            else
                            {
                                action_result.destination = active_dest_gport;
                            }
                        }
                        break;
                    }
                    case bcmOAMMatchTypeMIP:
                    {
                        /*
                         * Configure MIP: Only CCM, LBM and LTM are handled in MIP
                         *
                         *  Destination port table for MIP:
                         *  
                         *  Opcode |   UniCast   Multicast   UnknownUniCast
                         *  -----------------------------------------------
                         *  LBM    |    recycle    skip        skip
                         *  LTM    |    cpu        cpu         cpu
                         *  CCM    |    error      snoop       skip
                         *  Other  |    error      skip        skip
                         */
                        action_result.counter_increment = 0;
                        if (action_key.opcode == bcmOamOpcodeLTM)
                        {
                            /*
                             *  LTMs should be forwarded without updating the TTL,
                             *  where the CPU will be responsible for forwarding.
                             */
                            action_result.destination = cpu_dest_gport;
                        }
                        else if (action_key.opcode == bcmOamOpcodeLBM)
                        {
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                action_result.destination = recycle_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        else if (action_key.opcode == bcmOamOpcodeCCM)
                        {
                            /*
                             * CCM packets should be forwarded
                             */
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                /*
                                 * level_dest_port is used for error cases
                                 */
                                action_result.destination = level_dest_gport;
                            }
                            else if (dest_mac_type == bcmOAMDestMacTypeMcast)
                            {
                                action_result.destination = snoop_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        else
                        {
                            /*
                             * MIPs are transparent to all but CCM, LTM, LBM.
                             */
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                /*
                                 * level_dest_port is used for error cases
                                 */
                                action_result.destination = level_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        break;
                    }
                    case bcmOAMMatchTypePassive:
                        /*
                         * Set action for packets that match the MEP, on the
                         * opposite direction
                         */
                    {
                        action_result.destination = passive_dest_gport;
                        break;
                    }

                    case bcmOAMMatchTypeLowMDL:
                        /*
                         * Set action for packets that have lower level than the MEP
                         */
                    {
                        action_result.destination = level_dest_gport;
                        break;
                    }

                    default:
                    {
                        skip_config = 1;
                        break;
                    }
                }
                /*
                 * Set Action for profile
                 */
                if (skip_config == 0)
                {
                    flags = 0;
                    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
                    if (BCM_FAILURE(rv))
                    {
                        printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
                        return rv;
                    }
                }
            }
        }
    }

    return rv;
}

/*
 * Example of creating action profiles and assigning them to a lif 
 * 1. Create Ingress action profile 
 * 2. Create Engress action profile 
 * 3. Set loss action on ingress profile 
 * 4. Bind the profiles to a oam-lif
 */
int
oam_action_profile_create(
    int unit,
    uint8 is_ingress,
    bcm_gport_t oam_lif,
    bcm_oam_profile_t ingress_profile_id,
    bcm_oam_profile_t egress_profile_id,
    int is_dual_ended)
{
    int rv;
    int flags;
    bcm_oam_profile_type_t profile_type;

    rv = 0;

    /*
     * Create OAM ingress profile
     */
    flags = BCM_OAM_PROFILE_WITH_ID;
    if (is_dual_ended)
    {
        flags |= BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED;
    }
    profile_type = bcmOAMProfileIngressLIF;
    rv = bcm_oam_profile_create(unit, flags, profile_type, &ingress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_create.\n", rv);
        return rv;
    }
    /*
     * Create OAM egress profile
     */
    flags = BCM_OAM_PROFILE_WITH_ID;
    if (is_dual_ended)
    {
        flags |= BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED;
    }
    profile_type = bcmOAMProfileEgressLIF;
    rv = bcm_oam_profile_create(unit, flags, profile_type, &egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_create.\n", rv);
        return rv;
    }

    /*
     * Set ingress profile action
     */
    rv = oam_lm_action(unit, is_ingress, ingress_profile_id, is_dual_ended);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_lm_action.\n", rv);
        return rv;
    }

    /*
     * Bind profiles to oam_lif
     */
    rv = bcm_oam_lif_profile_set(unit, 0, oam_lif, ingress_profile_id, egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
        return rv;
    }
    return rv;
}

/*
 * Get set of destinations for oam 
 */
int
oam_get_destinations(
    int unit,
    oam_destinations_s * oam_destinations)
{
    int rv;
    bcm_gport_t trap_code;

    /*
     * Get the trap code dedicated for active down MEP. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapBfdOamDownMEP, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->active_down_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM Level - ingress. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamLevel, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->level_dest_gport_ingress, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM Level - egress. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamLevel, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->level_dest_gport_egress, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM Passive - Ingress. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPassive, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->passive_dest_gport_ingress, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM Passive - Egress. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamPassive, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->passive_dest_gport_egress, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM OAMP trap with  destination - Ethernet accelerated MEPs. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamEthAccelerated, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->oamp_eth_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM performance trap with OAMP destination - Ethernet accelerated MEPs.  
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceEthAccelerated, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->oamp_eth_performance_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM trap with OAMP destination - PWE accelerated MEPs. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamY1731Pwe, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->oamp_pwe_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM performance trap with OAMP destination - PWE accelerated MEPs. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceY1731Pwe, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->oamp_pwe_performance_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM trap with OAMP destination - MPLS accelerated MEPs. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamY1731MplsTp, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->oamp_mpls_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM performance trap with OAMP destination - MPLS accelerated MEPs.
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceY1731MplsTp, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->oamp_mpls_performance_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for OAM Snoop. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapSnoopOamPacket, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->snoop_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for CPU trap. 
     */
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapBfdOamDownMEP, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->cpu_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for CPU trap. 
     */
    rv = bcm_rx_trap_type_get(unit,0 , bcmRxTrapEgTxOamUpMEPDest1, &trap_code);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->active_up_dest_gport, trap_code, 10, 0);

    /*
     * Get the trap code dedicated for CPU Recycle. 
     */
    
/*   rv = bcm_rx_trap_type_get(unit,0 , , &trap_code);*/
    trap_code = 3;
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
    }
    BCM_GPORT_TRAP_SET(oam_destinations->recycle_dest_gport, trap_code, 10, 0);

    return rv;
}

/*
 * Example of creating action profiles
 * 1. Create Ingress action profile 
 * 2. Create Engress action profile 
 * 3. Set loss action on ingress profile 
 * 4. Inputs:
 *    - unit: relevant unit id
 *    - is_ingress: 1 to configure ingress action, 0 for egress
 *    - profile_id: Configured profile's id
 *    - oam_destinations: structure of destinations to be set in 
 *                        as action table. Include destinations for:
 *                                             active_down_dest_gport;
 *                                             active_up_dest_gport;
 *                                             snoop_dest_gport;
 *                                             recycle_dest_gport;
 *                                             cpu_dest_gport;
 *                                             level_dest_gport;
 *                                             oamp_eth_dest_gport;
 *                                             oamp_mpls_dest_gport;
 *                                             oamp_pwe_dest_gport;
 *                                             passive_dest_gport;
 *    - lif_type: ETH/MPLS/PWE
 *    - lm_type: Single Ended LM / Duam Ended LM / SLM
 */
int
oam_action_config(
    int unit,
    uint8 is_ingress,
    uint8 is_acc,
    bcm_oam_profile_t profile_id,
    oam_destinations_s * oam_destinations,
    int lif_type,
    int lm_type)
{
    bcm_error_t rv;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    int endpoint_type;
    int dest_mac_type;
    uint32 flags;
    int skip_config;
    int opcode_index;
    int nof_supported_opcodes;
    int endpoint_type_start;
    int endpoint_type_end;
    int nof_mac_types;
    bcm_rx_trap_config_t trap_config;
    int trap_id;
    bcm_gport_t active_dest_gport;
    bcm_gport_t oamp_dest_gport;
    bcm_gport_t oamp_performance_dest_gport;
    bcm_gport_t level_dest_gport;
    bcm_gport_t passive_dest_gport;

    rv = BCM_E_NONE;
    flags = 0;

    if (is_acc)
    {
        /*
         * Assign for oamp_dest_port according to lif_type
         */
        if (lif_type == MEP_TYPE_ETH)
        {
            oamp_dest_gport = oam_destinations->oamp_eth_dest_gport;
            oamp_performance_dest_gport = oam_destinations->oamp_eth_performance_dest_gport;
        }
        else if (lif_type == MEP_TYPE_MPLS)
        {
            oamp_dest_gport = oam_destinations->oamp_mpls_dest_gport;
            oamp_performance_dest_gport = oam_destinations->oamp_mpls_performance_dest_gport;
        }
        else
        {
            oamp_dest_gport = oam_destinations->oamp_pwe_dest_gport;
            oamp_performance_dest_gport = oam_destinations->oamp_pwe_performance_dest_gport;
        }
    }
    if (is_ingress)
    {
        active_dest_gport = oam_destinations->active_down_dest_gport;
        level_dest_gport = oam_destinations->level_dest_gport_ingress;
        passive_dest_gport = oam_destinations->passive_dest_gport_ingress;
    }
    else
    {
        active_dest_gport = oam_destinations->active_up_dest_gport;
        level_dest_gport = oam_destinations->level_dest_gport_egress;
        passive_dest_gport = oam_destinations->passive_dest_gport_egress;
    }

    /*
     * Prepare action_key and action_result 
     */
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = 0;

    action_result.flags = 0;
    action_result.meter_enable = 0;
    action_result.destination = BCM_GPORT_INVALID;

    nof_supported_opcodes = 15;
    nof_mac_types = 3;

    if (is_acc || (lif_type == MEP_TYPE_MPLS) || (lif_type == MEP_TYPE_PWE))
    {
        /*
         * enpoint_type is not a key for Acc-MEP-Action table. So endpoint_type_start = endpoint_type_end = bcmOAMMatchTypeMEP 
         */
        endpoint_type_start = bcmOAMMatchTypeMEP;
        endpoint_type_end = bcmOAMMatchTypeMEP;
    }
    else
    {
        /*
         *  For non-acc profile configure all endpoint types
         */
        endpoint_type_start = bcmOAMMatchTypeMIP;
        endpoint_type_end = bcmOAMMatchTypeLowMDL;
    }

    /*
     * Configure all mac types
     */
    for (dest_mac_type = 0; dest_mac_type < nof_mac_types; dest_mac_type++)
    {
        action_key.dest_mac_type = dest_mac_type;
        /*
         * Configure all supported OAM opcodes
         */
        for (opcode_index = 0; opcode_index < nof_supported_opcodes; opcode_index++)
        {
            action_key.opcode = supported_opcodes[opcode_index];

            /*
             * Configure all endpoint match types
             */
            for (endpoint_type = endpoint_type_start; endpoint_type <= endpoint_type_end; endpoint_type++)
            {
                /*
                 * Don't count OAM packets 
                 * (Except for CCM packets in LM-MEPs and 
                 *  for SLM packets in SLM-MEPs)
                 */
                action_result.counter_increment = 0;
                skip_config = 0;
                action_key.endpoint_type = endpoint_type;
                /*
                 *  Configure endpoint_type dependent variables 
                 *  Specific cases that are configured separately,
                 *  are skipped.
 */
                switch (endpoint_type)
                {
                    case bcmOAMMatchTypeMEP:
                    {
                        /*
                         * Set action for packets that match the MEP
                         */
                        /*
                         * Acc-MEP-Action-Dis-Cnt should always be disabled (counting enabled) 
                         * to support HLM by MDL: 
                         * Counting highest level nonCCM-OAM packets should be enabled 
                         * for lower level MEPs
                         */
                        if (!is_acc)
                        {
                            if ((lm_type == LM_TYPE_SINGLE_ENDED) && (action_key.opcode == bcmOamOpcodeCCM))
                            {
                                /*
                                 * For LM action count only OAM-CCM packets 
                                 */
                                action_result.counter_increment = 1;
                            }
                        }
                        else
                        {
                            action_result.counter_increment = 1;
                        }
                        /*
                         * Send matching packets to active_dest_port 
                         *  
                         *  Destination port table for NON-ACC-MEP:
                         *  For MPLS/PWE the packet will always use  
						 *  dest_mac_type=UnknownUniCast
                         *  
                         *                  ETHERNET                        |           MPLS/PWE
                         *  Opcode |   UniCast   Multicast   UnknownUniCast |   Multicast   UnknownUniCast
                         *  ------------------------------------------------|------------------------------
                         *  XXM    |    active     active      error        |    active        active
                         *  LBM    |    recycle    active      error        |    active        active
                         *  CCM    |    active     active      error        |    active        active
                         *  Other  |    active     error       error        |    active        active
                         *  
                         *  Destination port table for ACC-MEP:
                         *                                                  |
                         *         |        ETHERNET                        |           MPLS/PWE
                         *  Opcode |   UniCast   Multicast   UnknownUniCast |   Multicast   UnknownUniCast
                         *  ------------------------------------------------|-------------------------------
                         *  XXM    |    active     active      error        |    active        active
                         *  LBM    |    recycle    active      error        |    active        active
                         *  CCM    |    active     oamp        error        |    oamp          oamp
                         *  DMM    |    active     active      error        |    active        active
                         *  DMR    |    active     error       error        |    active        active
                         *  Other  |    active     error       error        |    active        active
                         */
                        if (lif_type == MEP_TYPE_ETH)
                        {
                            if (dest_mac_type == bcmOAMDestMacTypeUknownUcast)
                            {
                                action_result.destination = level_dest_gport;
                            }
                            else if (dest_mac_type == bcmOAMDestMacTypeMcast)
                            {
                                if ((action_key.opcode == bcmOamOpcodeLinearAPS)
                                    || (action_key.opcode == bcmOamOpcodeLTM) || (action_key.opcode == bcmOamOpcodeLBM)
                                    || (action_key.opcode == bcmOamOpcodeLMM) || (action_key.opcode == bcmOamOpcodeAIS)
                                    || (action_key.opcode == bcmOamOpcodeSLM) || (action_key.opcode == bcmOamOpcodeDMM))
                                {
                                    action_result.destination = active_dest_gport;
                                }
                                else if (action_key.opcode == bcmOamOpcodeCCM)
                                {
                                    if (is_acc)
                                    {
                                        action_result.destination = oamp_dest_gport;
                                    }
                                    else
                                    {
                                        action_result.destination = active_dest_gport;
                                    }
                                }
                                else
                                {
                                    action_result.destination = level_dest_gport;
                                }
                            }
                            else
                            {
                                /*
                                 * Unicast
                                 */
                                if ((action_key.opcode == bcmOamOpcodeLinearAPS)
                                    || (action_key.opcode == bcmOamOpcodeLTM) || (action_key.opcode == bcmOamOpcodeCCM)
                                    || (action_key.opcode == bcmOamOpcodeLMM) || (action_key.opcode == bcmOamOpcodeAIS)
                                    || (action_key.opcode == bcmOamOpcodeSLM) || (action_key.opcode == bcmOamOpcodeDMR) || (action_key.opcode == bcmOamOpcodeDMM))
                                {
                                    action_result.destination = active_dest_gport;
                                }
                                else if (action_key.opcode == bcmOamOpcodeLBM)
                                {
                                    action_result.destination = oam_destinations->recycle_dest_gport;
                                }
                                else
                                {
                                    action_result.destination = active_dest_gport;
                                }
                            }
                        }
                        else
                        {
                            /*
                             * MPLS / PWE
                             */
                            if ((dest_mac_type == bcmOAMDestMacTypeMcast) || (dest_mac_type == bcmOAMDestMacTypeUknownUcast))
                            {
                                if ((action_key.opcode == bcmOamOpcodeLinearAPS)
                                    || (action_key.opcode == bcmOamOpcodeLTM) || (action_key.opcode == bcmOamOpcodeLBM)
                                    || (action_key.opcode == bcmOamOpcodeLMM) || (action_key.opcode == bcmOamOpcodeAIS)
                                    || (action_key.opcode == bcmOamOpcodeSLM) || (action_key.opcode == bcmOamOpcodeDMM))
                                {
                                    action_result.destination = active_dest_gport;
                                }
                                else if (action_key.opcode == bcmOamOpcodeCCM)
                                {
                                    if(is_acc)
                                    {
                                        action_result.destination = oamp_dest_gport;
                                    }
                                    else
                                    {
                                        action_result.destination = active_dest_gport;
                                    }
                                }
                                else
                                {
                                    action_result.destination = active_dest_gport;
                                }
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        break;
                    }
                    case bcmOAMMatchTypeMIP:
                    {
                        /*
                         * Configure MIP: Only CCM, LBM and LTM are handled in MIP
                         *
                         *  Destination port table for MIP:
                         *  
                         *  Opcode |   UniCast   Multicast   UnknownUniCast
                         *  -----------------------------------------------
                         *  LBM    |    recycle    skip        skip
                         *  LTM    |    cpu        cpu         cpu
                         *  CCM    |    error      snoop       skip
                         *  Other  |    error      skip        skip
                         */
                        action_result.counter_increment = 0;
                        if (action_key.opcode == bcmOamOpcodeLTM)
                        {
                            /*
                             *  LTMs should be forwarded without updating the TTL,
                             *  where the CPU will be responsible for forwarding.
                             */
                            action_result.destination = oam_destinations->cpu_dest_gport;
                        }
                        else if (action_key.opcode == bcmOamOpcodeLBM)
                        {
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                action_result.destination = oam_destinations->recycle_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        else if (action_key.opcode == bcmOamOpcodeCCM)
                        {
                            /*
                             * CCM packets should be forwarded
                             */
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                /*
                                 * level_dest_port is used for error cases
                                 */
                                action_result.destination = level_dest_gport;
                            }
                            else if (dest_mac_type == bcmOAMDestMacTypeMcast)
                            {
                                action_result.destination = oam_destinations->snoop_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        else
                        {
                            /*
                             * MIPs are transparent to all but CCM, LTM, LBM.
                             */
                            if (dest_mac_type == bcmOAMDestMacTypeMyCfmMac)
                            {
                                /*
                                 * level_dest_port is used for error cases
                                 */
                                action_result.destination = level_dest_gport;
                            }
                            else
                            {
                                skip_config = 1;
                            }
                        }
                        break;
                    }
                    case bcmOAMMatchTypePassive:
                        /*
                         * Set action for packets that match the MEP, on the
                         * opposite direction
                         */
                    {
                        action_result.destination = passive_dest_gport;
                        break;

                    }

                    case bcmOAMMatchTypeLowMDL:
                        /*
                         * Set action for packets that have lower level than the MEP
                         */
                    {
                        action_result.destination = level_dest_gport;
                        break;

                    }

                    default:
                    {
                        skip_config = 1;
                        break;
                    }
                }
                /*
                 * Set Action for profile
                 */
                if (skip_config == 0)
                {
                    flags = 0;
                    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
                    if (BCM_FAILURE(rv))
                    {
                        printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
                        return rv;
                    }
                }
            }
        }
    }

    /*
     * Action for INJECTed packets 
     */
    /*
     * endpoint_type = bcmOAMMatchTypeMEP, mac_type = bcmOAMDestMacTypeMcast
     * 
     *                  |      CCM       DMM/DMR       Other
     *  ------------------------------------------------------
     *  Single Ended    |    count    1588 no_count    skip
     *  Dual Ended      |    skip     1588 no_count    skip
     *  SLM             |    skip     1588 no_count    skip
     */

    if (lm_type == LM_TYPE_SINGLE_ENDED)
    {
        /*
         * Prepare action_key and action_result 
 */
        action_key.flags = 0;
        action_key.inject = 1;
        action_key.opcode = 0;
        action_key.endpoint_type = bcmOAMMatchTypeMEP;
        action_key.opcode = bcmOamOpcodeCCM;
        action_key.dest_mac_type = bcmOAMDestMacTypeMcast;

        action_result.flags = 0;
        action_result.meter_enable = 0;
        action_result.destination = BCM_GPORT_INVALID;
        action_result.counter_increment = 1;

        flags = 0;
        rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
        if (BCM_FAILURE(rv))
        {
            printf("bcm_dnx_oam_profile_action_set failed. Error:%d \n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * Example of creating action profiles
 * Set 3 action profiles - for single ended, dual ended and slm 
 * 1. Create Ingress action profile
 * 2. Create Engress action profile 
 * 3. Set loss action on ingress profile 
 *  
 * As a result the LIF-Action table will be configured as follows: 
 * 
 * ================================================================================= 
 *    Profile No  ||         Profile Name         || LIF TYPE  | LM TYPE 
 * ================================================================================= 
 *       2        || oam_profile_eth_single_ended  ||   ETH    |   Single Ended
 *       3        || oam_profile_eth_dual_ended    ||   ETH    |   Dual Ended
 *       4        || oam_profile_eth_none          ||   ETH    |   NONE
 *       5        || oam_profile_mpls_single_ended ||   MPLS   |   Single Ended
 *       6        || oam_profile_mpls_dual_ended   ||   MPLS   |   Dual Ended
 *       7        || oam_profile_mpls_none         ||   MPLS   |   NONE
 *       8        || oam_profile_pwe_single_ended  ||   PWE    |   Single Ended
 *       9        || oam_profile_pwe_dual_ended    ||   PWE    |   Dual Ended
 *       10        || oam_profile_pwe_none         ||   PWE    |   NONE
 */
int
oam_non_acc_set_of_action_profiles_create(
    int unit,
    oam_destinations_s oam_destinations)
{
    int rv;
    int is_acc;
    int flags;
    int lm_type;
    int lif_type;
    int lif_profile_index;
    bcm_oam_profile_type_t profile_type;
    bcm_oam_profile_t ingress_profile_id;
    bcm_oam_profile_t egress_profile_id;
    uint8 is_ingress;
    uint8 is_egress;

    rv = 0;
    is_acc = 0;
    lif_profile_index = 0;
    is_ingress = 1;
    is_egress = 0;

    for (lif_type = MEP_TYPE_ETH; lif_type <= MEP_TYPE_PWE; lif_type++)
    {
        for (lm_type = LM_TYPE_SINGLE_ENDED; lm_type <= LM_TYPE_NONE; lm_type++)
        {
            /*
             * Create OAM ingress profile
             */
             flags=0;
             if(hierarchical_lm_disable)
             {
                 flags |= BCM_OAM_PROFILE_HIERARCHICAL_LM_DISABLE;
             }

            if (lm_type == LM_TYPE_DUAL_ENDED)
            {
                flags |= BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED;
            }
            else if (lm_type == LM_TYPE_NONE)
            {
                flags |= BCM_OAM_PROFILE_LM_TYPE_NONE;
            }

            profile_type = bcmOAMProfileIngressLIF;
            rv = bcm_oam_profile_create(unit, flags, profile_type, &ingress_profile_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error no %d, in bcm_oam_profile_create.\n", rv);
                return rv;
            }
            *(oam_lif_profiles_t[lif_profile_index]) = ingress_profile_id;
            /*
             * Create OAM egress profile
             */
            if (lif_type == MEP_TYPE_ETH)
            {
                profile_type = bcmOAMProfileEgressLIF;
                rv = bcm_oam_profile_create(unit, flags, profile_type, &egress_profile_id);
                if (rv != BCM_E_NONE)
                {
                    printf("Error no %d, in bcm_oam_profile_create.\n", rv);
                    return rv;
                }
            }

            /*
             * Set ingress profile action
             */
            rv = oam_action_config(unit, is_ingress, is_acc, ingress_profile_id, oam_destinations, lif_type, lm_type);
            if (rv != BCM_E_NONE)
            {
                printf("Error no %d, in oam_action_config.\n", rv);
                return rv;
            }

            if (lif_type == MEP_TYPE_ETH)
            {
               /*
                * Set egress profile action
                */
               rv = oam_action_config(unit, is_egress, is_acc, egress_profile_id, oam_destinations, lif_type, lm_type);
               if (rv != BCM_E_NONE)
               {
                   printf("Error no %d, in oam_action_config.\n", rv);
                   return rv;
               }
            }
            lif_profile_index++;
        }
    }

    return rv;
}

/*
 * Example of creating action profiles
 * Set action profiles for 3 lif types and 2 lm_types -
 *     for single ended and dual ended,
 *     for ETH,MPLS,PWE 
 * 1. Create Ingress action profile
 * 2. Create Engress action profile 
 * 3. Set loss action on ingress profile 
 *  
 * As a result the Acc-MEP-Action table will be configured as follows: 
 *  
 * ===============================================================================
 * Acc Profile No ||           Profile Name            || LIF TYPE | LM TYPE 
 * ===============================================================================
 *       2        || oam_acc_profile_eth_single_ended  ||  ETH    |   Single Ended
 *       3        || oam_acc_profile_eth_dual_ended    ||  ETH    |   Dual Ended
 *       4        || oam_acc_profile_eth_none          ||  ETH    |   NONE
 *       5        || oam_acc_profile_mpls_single_ended ||  MPLS   |   Single Ended
 *       6        || oam_acc_profile_mpls_dual_ended   ||  MPLS   |   Dual Ended
 *       7        || oam_acc_profile_mpls_none         ||  MPLS   |   NONE
 *       8        || oam_acc_profile_pwe_single_ended  ||  PWE    |   Single Ended
 *       9        || oam_acc_profile_pwe_dual_ended    ||  PWE    |   Dual Ended
 *      10        || oam_acc_profile_pwe_none          ||  PWE    |   NONE
 */
int
oam_acc_set_of_action_profiles_create(
    int unit,
    uint8 is_ingress,
    oam_destinations_s oam_destinations)
{
    int rv;
    int is_acc;
    int flags;
    int lm_type;
    int lif_type;
    int lif_profile_index;
    bcm_oam_profile_type_t profile_type;
    bcm_oam_profile_t ingress_profile_id;
    bcm_oam_profile_t egress_profile_id;

    rv = 0;
    is_acc = 1;
    lif_profile_index = 0;

    for (lif_type = MEP_TYPE_ETH; lif_type <= MEP_TYPE_PWE; lif_type++)
    {
        for (lm_type = LM_TYPE_SINGLE_ENDED; lm_type <= LM_TYPE_NONE; lm_type++)
        {
           /*
            * Create OAM ingress profile
            */
            flags=0;
            if(hierarchical_lm_disable)
            {
                flags |= BCM_OAM_PROFILE_HIERARCHICAL_LM_DISABLE;
            }

           if (lm_type == LM_TYPE_DUAL_ENDED)
           {
               flags |= BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED;
           }
           else if (lm_type == LM_TYPE_NONE) 
           {
              flags |= BCM_OAM_PROFILE_LM_TYPE_NONE;
           }

           profile_type = bcmOAMProfileIngressAcceleratedEndpoint;
           rv = bcm_oam_profile_create(unit, flags, profile_type, &ingress_profile_id);
           if (rv != BCM_E_NONE)
           {
               printf("Error no %d, in bcm_oam_profile_create.\n", rv);
               return rv;
           }
           *(oam_acc_lif_profiles_t[lif_profile_index]) = ingress_profile_id;
           /*
            * Create OAM egress profile for ETH
            */
           if (lif_type == MEP_TYPE_ETH)
           {
               profile_type = bcmOAMProfileEgressAcceleratedEndpoint;
               rv = bcm_oam_profile_create(unit, flags, profile_type, &egress_profile_id);
               if (rv != BCM_E_NONE)
               {
                   printf("Error no %d, in bcm_oam_profile_create.\n", rv);
                   return rv;
               }
           }

           /*
            * Set ingress profile action
            */
           rv = oam_action_config(unit, is_ingress, is_acc, ingress_profile_id, oam_destinations, lif_type, lm_type);
           if (rv != BCM_E_NONE)
           {
               printf("Error no %d, in oam_action_config.\n", rv);
               return rv;
           }
           lif_profile_index++;
        }
    }

    return rv;
}

/*
 * Example of creating action profiles
 * 1. Create Ingress action profile 
 * 2. Create Engress action profile 
 * 3. Set loss action on ingress profile 
 */
int
oam_set_of_action_profiles_create(
    int unit)
{
    int rv;
    int index;
    int is_acc;
    uint8 is_ingress;
    oam_destinations_s oam_destinations;

    rv = 0;
    is_acc = 0;
    is_ingress = 1;

    /*
     * Get Destinations
     */
    rv = oam_get_destinations(unit, &oam_destinations);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_get_destinations.\n", rv);
        return rv;
    }

    rv = oam_non_acc_set_of_action_profiles_create(unit, oam_destinations);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_non_acc_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    rv = oam_acc_set_of_action_profiles_create(unit, is_ingress, oam_destinations);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_acc_set_of_action_profiles_create.\n", rv);
        return rv;
    }
    return rv;
}


/*
 * Example of creating section action profiles 
 * ------------------------------------------- 
 *  
 * In OAM Section, altough there is single lif per packet 
 * all 3 LIF_DB accesses hit. Therefore we need a special 
 * OAM profile to control LM counting. 
 *  
 * 1. Get trap destinations
 * 2. Create Ingress action profile 
 * 3. Set loss action on ingress profile
 * 4. Create Ingress accelerated action profile 
 * 5. Set loss action on ingress accelerated profile 
 *  
 *  
 * Inputs: 
 * lm_type: Single Ended LM (0)/ Duam Ended LM (1)/ SLM (2) 
 * Outputs: 
 *  ingress_profile_id - OAM profile id
 *  acc_ingress_profile - OAM accelerated profile id
 */
int
oam_section_action_profile_create(
    int unit,
    int lm_type,
    bcm_oam_profile_t *ingress_profile_id,
    bcm_oam_profile_t *acc_ingress_profile)
{
    int rv;
    int is_acc;
    int flags;
    int lif_type;
    int lif_profile_index;
    bcm_oam_profile_type_t profile_type;
    uint8 is_ingress;
    uint8 is_egress;
    oam_destinations_s oam_destinations;

    rv = 0;
    is_acc = 0;
    lif_profile_index = 0;
    is_ingress = 1;
    is_egress = 0;

    lif_type = MEP_TYPE_PWE;

    /*
     * Get Destinations
     */
    rv = oam_get_destinations(unit, &oam_destinations);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_get_destinations.\n", rv);
        return rv;
    }

   flags = BCM_OAM_PROFILE_TYPE_SECTION;
   if (lm_type == LM_TYPE_DUAL_ENDED)
   {
       flags |= BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED;
   }
   else if (lm_type == LM_TYPE_NONE)
   {
       flags |= BCM_OAM_PROFILE_LM_TYPE_NONE;
   }

   /*
    * Create OAM ingress profile
    */
   profile_type = bcmOAMProfileIngressLIF;
   rv = bcm_oam_profile_create(unit, flags, profile_type, ingress_profile_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_profile_create.\n", rv);
       return rv;
   }
   oam_section_profile = *ingress_profile_id;

   /*
    * Set ingress profile action
    */
   rv = oam_action_config(unit, is_ingress, is_acc, *ingress_profile_id, oam_destinations, lif_type, lm_type);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in oam_action_config.\n", rv);
       return rv;
   }

   /*
    * Create OAM ingress accelerated profile
    */
   is_acc = 1;
   profile_type = bcmOAMProfileIngressAcceleratedEndpoint;
   rv = bcm_oam_profile_create(unit, flags, profile_type, acc_ingress_profile);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_profile_create.\n", rv);
       return rv;
   }
   oam_section_acc_profile = *acc_ingress_profile;
   /*
    * Set ingress profile action
    */
   rv = oam_action_config(unit, is_ingress, is_acc, *acc_ingress_profile, oam_destinations, lif_type, lm_type);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in oam_action_config.\n", rv);
       return rv;
   }

    return rv;
}

/**
 * This routne will be used to set the OAM opcode destination to OAMP
 * The OAMP may reply to LMMs, DMMs, LBMs, LTMs and EXMs, SLMs with
 * their respective reply packets.
 * This is an example of how to set the reply mechanism.
 *
 * OAMP can also handle DMRs, LMRs after loss/delay_add has been called.
 *
 * The reply packet's DA will be taken from the incoming
 * packet's SA.
 * The reply packets SA will be as configured in the
 * endpoint_create() API.
 *
 * Note that LMRs, DMRs, SLRs will be transmitted with the
 * counters/ToD stamped on the packet. For other types of reply
 * packets, only the OpCode will change.
 *
 * @param unit
 * @param is_ingress
 * @param acc_profile_id - raw profile (integer between 1 to 15)
 *        the action will apply to all endpoints that use this profile
 * @profile opcode - LMM, DMM, LBM, LTM or SLM
 * @is_mpls - is the action intended for mpls packets
 * @is_pwe - is the action intended for pwe packets
 *
 * @return error code
 */

int oam_set_opcode_destination_to_oamp(int unit,int is_ingress ,int acc_profile_id, int opcode, bcm_oam_endpoint_type_t type) {

    bcm_error_t rv;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    int dest_mac_type;
    int trap_code;
    bcm_gport_t oamp_dest_gport;
    unsigned int flags = 0;
    bcm_oam_profile_t enc_profile_id;

    bcm_oam_action_key_t_init(&action_key);
    bcm_oam_action_result_t_init(&action_result);

    /*
     *  Get encoded oam profile id from raw OAM profile id and OAM profile type
     */

    rv = bcm_oam_profile_id_get_by_type(unit, acc_profile_id,
        is_ingress ? bcmOAMProfileIngressAcceleratedEndpoint : bcmOAMProfileEgressAcceleratedEndpoint, &flags, &enc_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_oam_profile_id_get_by_type failed with error: (%s) \n", bcm_errmsg(rv));
        return rv;
    }

    if (type == bcmOAMEndpointTypeBHHMPLS) {
        if (opcode == bcmOamOpcodeLMR || opcode == bcmOamOpcodeDMR || opcode == bcmOamOpcode1DM || opcode == bcmOamOpcodeLMM || opcode == bcmOamOpcodeDMM)
        {
           /** Set trap for Performance PDUs */ 
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceY1731MplsTp, &trap_code);
        }
        else
        {
             rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamY1731MplsTp, &trap_code);
        }
    } else if (type == bcmOAMEndpointTypeBHHPwe) {
        if (opcode == bcmOamOpcodeLMR || opcode == bcmOamOpcodeDMR || opcode == bcmOamOpcode1DM || opcode == bcmOamOpcodeLMM || opcode == bcmOamOpcodeDMM)
        {
            /** Set trap for Performance PDUs */  
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceY1731Pwe, &trap_code);
        }
        else
        {
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamY1731Pwe, &trap_code);
        }
    } else if (is_ingress && type == bcmOAMEndpointTypeEthernet) {
        if (opcode == bcmOamOpcodeLMR || opcode == bcmOamOpcodeDMR || opcode == bcmOamOpcode1DM || opcode == bcmOamOpcodeLMM || opcode == bcmOamOpcodeDMM)
        {
            /** Set trap for Performance PDUs */ 
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceEthAccelerated, &trap_code);
        }
        else
        {
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamEthAccelerated, &trap_code);
        }
    } else if (type == bcmOAMEndpointTypeEthernet) {
        /** In case of pcp count enabled, different egress trap is required */
        if (is_pcp_counting_enabled)
        {
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &trap_code);
        }
        else
        {
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPOamp, &trap_code);
        }
    } else if (type == bcmOAMEndpointTypeMplsLmDmLsp || type == bcmOAMEndpointTypeMplsLmDmPw || type == bcmOAMEndpointTypeMplsLmDmSection) {
        if (opcode == bcmOamMplsLmDmOpcodeTypeLm)
        {
            /**  MplsLm trap code should be bcmRxTrapOamBfdPwe */
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamBfdPwe, &trap_code);
        }
        if ((opcode == bcmOamMplsLmDmOpcodeTypeDm) && (type == bcmOAMEndpointTypeMplsLmDmLsp))
        {
            /** Set trap for MplsDm PDUs over LSP */
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceY1731MplsTp, &trap_code);
        }
        if ((opcode == bcmOamMplsLmDmOpcodeTypeDm) && (type == bcmOAMEndpointTypeMplsLmDmPw))
        {
            /** Set trap for MplsDm PDUs over PWE  */
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceY1731Pwe, &trap_code);
        }
        if ((opcode == bcmOamMplsLmDmOpcodeTypeDm) && (type == bcmOAMEndpointTypeMplsLmDmSection))
        {
            /** Set trap for MplsDm PDUs over Section  */
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamPerformanceY1731Pwe, &trap_code);
        }
    } else {
        rv = BCM_E_PARAM;
        printf("type %d is not supported \n", type);
    }

    if (rv != BCM_E_NONE)
    {
        printf("bcm_rx_trap_type_get failed with error: (%s) \n", bcm_errmsg(rv));
        return rv;
    }
    BCM_GPORT_TRAP_SET(oamp_dest_gport, trap_code, 7, 0);

    action_key.flags = 0;
    if (type == bcmOAMEndpointTypeMplsLmDmLsp || type == bcmOAMEndpointTypeMplsLmDmPw || type == bcmOAMEndpointTypeMplsLmDmSection)
    {
        /** In case of MPLS LM/DM EP type, the opcode should be set through mpls_lm_dm_opcode with BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE flag on. */
        action_key.flags = BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE;
        action_key.mpls_lm_dm_opcode = opcode;
        action_key.opcode = 0;
    }
    else
    {
        action_key.opcode = opcode;
    }
    action_key.inject = 0;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;
    action_key.dest_mac_type = bcmOAMDestMacTypeMcast;

    action_result.flags = 0;
    action_result.meter_enable = 0;
    action_result.destination = oamp_dest_gport;
    if (opcode != bcmOamOpcodeCCM)
    {
        /** The counter-increment indication should only come form the non-acclerated profile */
        /** Not required for CCM PG-back */
        action_result.counter_increment = 1;
    }


    if (type == bcmOAMEndpointTypeEthernet) {
        /*
         * Set action to the profile obtained in the first step for Multicast
         */

        rv = bcm_oam_profile_action_set(unit, 0, enc_profile_id, &action_key, &action_result);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_oam_profile_action_set failed with error: (%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    if (type == bcmOAMEndpointTypeEthernet) {
        /*
         * Set action to the profile obtained in the first step for MyCfmMac
         */
        action_key.dest_mac_type = bcmOAMDestMacTypeMyCfmMac;
    } else {
        action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    }

    rv = bcm_oam_profile_action_set(unit, 0, enc_profile_id, &action_key, &action_result);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_oam_profile_action_set failed with error: (%s) \n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * OAM profile action set calling sequence example
 *
 * @param unit
 * @param opcode
 * @param dest_mac_type
 * @param endpoint_type
 * @param is_inject
 * @param counter_increment
 * @param meter_enable
 * @param dest_port - Must be BCM_GPORT_INVALID for actions requiring
 *                    invalid gport. Otherwise, trap.
 * @param is_ingress - 1 to configure ingress action, 0 for egress
 * @param profile_id - profile created with bcm_oam_profile_create()
 * of type:
 * - bcmOAMProfileIngressLIF for non-accelerated Down-MEPs
 * - bcmOAMProfileEgressLIF for non-accelerated Up-MEPs
 * - bcmOAMProfileIngressAcceleratedEndpoint for accelerated Down-MEPs
 * - bcmOAMProfileEgressAcceleratedEndpoint for accelerated Up-MEPs
 *
 * @return error code
 */
int dnx_oam_action_set(
    int unit,
    bcm_oam_opcode_t opcode,
    bcm_oam_dest_mac_type_t dest_mac_type,
    bcm_oam_match_type_t endpoint_type,
    uint8 is_inject,
    uint8 counter_increment,
    uint8 meter_enable,
    int dest_port,
    uint8 is_ingress,
    bcm_oam_profile_t profile_id)
{
    bcm_error_t rv;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    bcm_rx_trap_config_t trap_config;
    int ing_trap_code, egr_trap_code;
    int trap_strength = 15;
    int snoop_strength = 0;
    int flags = 0;

    bcm_oam_action_key_t_init(&action_key);
    bcm_oam_action_result_t_init(&action_result);

    if (dest_port != BCM_GPORT_INVALID && !BCM_GPORT_IS_TRAP(dest_port))
    {
        /* action_result.destination can only receive trap as destination. Allocate new trap */
        /* Set up new valid CPU port */
        rv = oam__switch_contol_indexed_port_set_to_cpu(unit, dest_port);
        if (rv != BCM_E_NONE)
        {
            printf("Setting port of type CPU failed: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /* Create ingress user defined trap */
        rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &ing_trap_code);
        if (rv != BCM_E_NONE)
        {
           printf("Error in bcm_rx_trap_type_create: (%s)\n", bcm_errmsg(rv));
           return rv;
        }

        /* Configure the new trap */
        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
        trap_config.dest_port = dest_port;
        rv = bcm_rx_trap_set(unit, ing_trap_code, trap_config);
        if (rv != BCM_E_NONE)
        {
           printf("Error in bcm_rx_trap_set: (%s)\n", bcm_errmsg(rv));
           return rv;
        }

        oam_user_defined_trap_id = ing_trap_code;
        printf("Trap set, trap_code = %d \n", ing_trap_code);

        if (is_ingress)
        {
            /* Encode ingress trap, trap strength and snoop strength as a gport */
            BCM_GPORT_TRAP_SET(action_result.destination, ing_trap_code, trap_strength, snoop_strength);
        }
        else
        {
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
            BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, ing_trap_code, trap_strength, snoop_strength);
            rv = bcm_rx_trap_set(unit, egr_trap_code, trap_config);
            if (rv != BCM_E_NONE)
            {
               printf("Error in bcm_rx_trap_set: (%s)\n", bcm_errmsg(rv));
               return rv;
            }

            printf("Trap set, trap_code = %d \n", egr_trap_code);

            /* Encode trap, trap strength and snoop strength as a gport */
            BCM_GPORT_TRAP_SET(action_result.destination, egr_trap_code, trap_strength, snoop_strength);
        }
    }
    else
    {
        /* Destination is trap */
        action_result.destination = dest_port;
    }

    action_key.flags = 0;
    action_key.inject = is_inject;
    action_key.opcode = opcode;
    action_key.endpoint_type = endpoint_type;
    action_key.dest_mac_type = dest_mac_type;

    action_result.flags = 0;
    action_result.counter_increment = counter_increment;
    action_result.meter_enable = meter_enable;

    /* Update the action for the given profile */
    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_profile_action_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Creating OAM egress trap for pcp counting calling sequence example
 *
 * @param unit
 * @return error code
 */
int dnx_oam_action_set_performance_up_mep_trap(
    int unit)
{
    int rv;
    void *dest_char;
    bcm_field_context_info_t context_info;
    bcm_field_context_param_info_t context_param;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t presel_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_gport_t oamp_gport[2];
    bcm_rx_trap_config_t trap_config;
    int count;
    int flags = 0;
    int up_mep_ingress_trap_id;
    int up_mep_ingress_trap_id2;
    int oamp_eth_performance_trap_id;

    /*
     * Configuring destination(OAMP) for accelerated endpoints
    */
    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gport, &count);
    if (rv != BCM_E_NONE) {
        printf("1(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /** Define User defined traps(on ingress side) in order not to add system headers on 2'nd pass */
    rv = bcm_rx_trap_type_get(unit,flags,bcmRxTrapOamPerformanceEthAccelerated,oamp_eth_performance_trap_id);
    if (rv != BCM_E_NONE) {
        printf("2(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.dest_port = oamp_gport[0];
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &up_mep_ingress_trap_id);
    if (rv != BCM_E_NONE) {
        printf("3(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_rx_trap_set(unit, up_mep_ingress_trap_id, trap_config);
    if (rv != BCM_E_NONE) {
        printf("4(%s) \n", bcm_errmsg(rv));
        return rv;
    }
 
    bcm_rx_trap_config_t_init(&trap_config);
    /** Ingress trap strength range 0-15 */
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, up_mep_ingress_trap_id, 15, 0);
    /** Trap stamped on FHEI header */
    trap_config.stamped_trap_code = oamp_eth_performance_trap_id;
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapEgTxOamUpMEPDest2, &up_mep_ingress_trap_id2);
    if (rv != BCM_E_NONE) {
        printf("5(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_rx_trap_set(unit, up_mep_ingress_trap_id2, trap_config);
    if (rv != BCM_E_NONE) {
        printf("6(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "OAM_No_Header", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE) {
        printf("7(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /** Changing new context SYS HDR profile to None (No SYS HDRs are added) */
    context_param.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    context_param.param_arg = 0;
    context_param.param_val = bcmFieldSystemHeaderProfileNone;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &context_param);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    context_param.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    context_param.param_arg = 0;
    context_param.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerOffset0, 0);
    /**Context that was created for iPMF1 is also created for iPMF2*/
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &context_param);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /** Adding trap_code preselector for the context */
    presel_entry_id.presel_id = 3;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    presel_data.entry_valid = TRUE;
    presel_data.context_id = context_id;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type = bcmFieldQualifyRxTrapCode;
    presel_data.qual_data[0].qual_value = up_mep_ingress_trap_id;
    presel_data.qual_data[0].qual_mask = 0x1FF;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_data);
    if (rv != BCM_E_NONE) {
        printf("6(%s) \n", bcm_errmsg(rv));
        return rv;
    }
     printf("Done\n");
    return 0;
}
