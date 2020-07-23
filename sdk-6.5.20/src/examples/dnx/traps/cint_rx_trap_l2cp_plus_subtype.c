/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File: cint_rx_trap_l2cp_plus_subtype.c
 * PORTED FROM: cint_l2_cache.c -> l2_cache_run_with_defaults
 * Purpose: Shows an example for configuration of L2CP and Programmable Dest Mac plus Subtype trap.
 *
 * Helper functions:
 * cint_rx_trap_l2cp_plus_subtype_l2cp_trap(unit, inP, outP);
 * cint_rx_trap_l2cp_plus_subtype_programmable_subtype_trap(unit, inP, outP);
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_l2cp_plus_subtype.c
 *
 * Main Function:
 *      cint_rx_trap_l2cp_plus_subtype_main(unit,inP,outP);
 * Destroy Function:
 *      cint_rx_trap_l2cp_plus_subtype_destroy(unit);
 *
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_l2cp_plus_subtype.c
 * cint
 * cint_rx_trap_l2cp_plus_subtype_main(0,200,202);
 */

int cint_rx_trap_l2cp_plus_subtype_l2cp_trap_id;
int cint_rx_trap_l2cp_plus_subtype_prog_subtype_trap_id;

uint8 cint_rx_trap_l2cp_plus_subtype_sub_type           = 0x0A;
uint8 cint_rx_trap_l2cp_plus_subtype_sub_type_mask      = 0xFF;

bcm_mac_t cint_rx_trap_l2cp_plus_subtype_dest_mac       = {0x01, 0x88, 0xc2, 0x00, 0x55, 0x00};
uint8 cint_rx_trap_l2cp_plus_subtype_dest_mac_nof_bits  = 40;


/**
* \brief
* Creates a UserDefined Ingress trap and sets a L2CP Protocol trap
* with action redirect and update TC to it.
*
* \par DIRECT INPUT:
*   \param [in] unit     - unit Id
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_plus_subtype_l2cp_trap(
   int unit,
   int inP,
   int outP)
{
    int rv = BCM_E_NONE;
    int l2cp_trap_profile = 0;
    int fwd_strength = 7, snp_strength = 0;
    bcm_gport_t trap_gport;
    bcm_rx_trap_protocol_key_t key ;
    bcm_rx_trap_protocol_profiles_t protocol_profiles = { 0 };
    bcm_rx_trap_config_t trap_config;

    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_PRIO;
    trap_config.prio = 2;
    trap_config.dest_port = outP;

    rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrapUserDefine, &trap_config, &cint_rx_trap_l2cp_plus_subtype_l2cp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_utils_rx_trap_create_and_set\n");
        return rv;
    }

    BCM_GPORT_TRAP_SET(trap_gport, cint_rx_trap_l2cp_plus_subtype_l2cp_trap_id, fwd_strength, snp_strength);

    /*
     * L2cp arguments varies from 0-63 according to mac 6 LSBs, example of 6 LSB = 000010
     */
    key.protocol_type = bcmRxTrapL2cpPeer;
    key.trap_args = 2;
    key.protocol_trap_profile = l2cp_trap_profile;

    protocol_profiles.l2cp_trap_profile = l2cp_trap_profile;

    rv = bcm_rx_trap_protocol_profiles_set(unit, inP, &protocol_profiles);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_protocol_profiles_set\n");
        return rv;
    }

    rv = bcm_rx_trap_protocol_set(unit, &key, trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_protocol_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
* Creates a UserDefined Ingress trap and sets a Programmable trap,
* which has 40 MSB of the Dest MAC to match and Subtype = 10.
* Packets that match will be dropped.
*
* \par DIRECT INPUT:
*   \param [in] unit     - unit Id
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_plus_subtype_programmable_subtype_trap(
   int unit,
   int inP,
   int outP)
{
    int rv = BCM_E_NONE;
    int l2cp_trap_profile = 0;
    int fwd_strength = 7, snp_strength = 0;
    bcm_gport_t trap_gport;
    bcm_rx_trap_prog_config_t prog_config;

    bcm_rx_trap_prog_config_t_init(&prog_config);

    rv = cint_utils_rx_trap_ingress_create_and_set_to_drop(unit, 0, bcmRxTrapUserDefine, 0, &cint_rx_trap_l2cp_plus_subtype_prog_subtype_trap_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error in cint_utils_rx_trap_create_and_set_to_drop\n");
      return rv;
    }

    BCM_GPORT_TRAP_SET(prog_config.trap_gport, cint_rx_trap_l2cp_plus_subtype_prog_subtype_trap_id, fwd_strength, snp_strength);

     /* Fill the prog_config sturct with Eth filtering info only */
    prog_config.dest_mac = cint_rx_trap_l2cp_plus_subtype_dest_mac;
    prog_config.dest_mac_nof_bits = cint_rx_trap_l2cp_plus_subtype_dest_mac_nof_bits;
    prog_config.dest_mac_enable = bcmRxTrapProgEnableMatch;

    prog_config.sub_type = cint_rx_trap_l2cp_plus_subtype_sub_type;
    prog_config.sub_type_mask = cint_rx_trap_l2cp_plus_subtype_sub_type_mask;
    prog_config.sub_type_enable = bcmRxTrapProgEnableMatch;


    rv = bcm_rx_trap_prog_set(unit, 0, 1, prog_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_prog_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
* Main function of the cint which calls the Programmable and Protocol trap create functions.
* \par DIRECT INPUT:
*   \param [in] unit     - unit Id
*   \param [in] inP      - In Port
*   \param [in] outP     - Out Port
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_plus_subtype_main(
    int unit,
    int inP,
    int outP)
{
    int rv = BCM_E_NONE;

    rv = cint_rx_trap_l2cp_plus_subtype_l2cp_trap(unit, inP, outP);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_l2cp_plus_subtype_l2cp_trap\n");
        return rv;
    }

    rv = cint_rx_trap_l2cp_plus_subtype_programmable_subtype_trap(unit, inP, outP);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_l2cp_plus_subtype_programmable_subtype_trap\n");
        return rv;
    }

    return rv;
}

/**
* \brief
* Destroys the programmable and protocol traps created from the main function.
* Also destroy the user defined trap IDs.
* \par DIRECT INPUT:
*   \param [in] unit     - unit Id
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_l2cp_plus_subtype_destroy(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_prog_config_t prog_config;
    bcm_rx_trap_protocol_key_t key;

    bcm_rx_trap_prog_config_t_init(&prog_config);

    BCM_GPORT_TRAP_SET(prog_config.trap_gport, cint_rx_trap_l2cp_plus_subtype_prog_subtype_trap_id, 0, 0);

    rv = bcm_rx_trap_prog_set(unit, 0, 1, prog_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_prog_set ETH Index\n");
        return rv;
    }

    key.protocol_type = bcmRxTrapL2cpPeer;
    key.trap_args = 2;
    key.protocol_trap_profile = 0;

    rv = bcm_rx_trap_protocol_clear(unit,&key);
    if (rv != BCM_E_NONE)
   {
       printf("Error in bcm_rx_trap_protocol_clear\n");
       return rv;
   }

    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_l2cp_plus_subtype_prog_subtype_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy \n");
        return rv;
    }

    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_l2cp_plus_subtype_l2cp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy \n");
        return rv;
    }

    return rv;
}
