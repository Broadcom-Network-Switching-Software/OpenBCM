/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File: cint_rx_trap_etpp_mtu_trap_rif.c
 * Purpose: Configuration that creates an ETPP User defined trap.
 *          Creates an MTU violation configuration PER RIF and attaches it,
 *          to the created ETPP trap.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_mtu_trap_rif.c
 *
 * Main Function:
 *      cint_rx_trap_etpp_mtu_trap_lif_main(unit,out_rif,fwd_layer_type,compressed_layer_type,trap_id,fwd_strength,snp_strength,mtu_val);
 * Destroy Function:
 *      cint_rx_trap_etpp_mtu_trap_lif_destroy(unit,out_rif,fwd_layer_type,compressed_layer_type);
 *
 * Example Config requires a valid(created) out_rif to be supplied:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_mtu_trap_rif.c
 * cint
 * cint_rx_trap_etpp_mtu_trap_lif_main(0,out_rif,3,3,4,7,0,0x20);
 */

/* The encoded User defined trap id */
int cint_etpp_mtu_trap_rif_trap_id;
bcm_rx_trap_config_t trap_config = {0};
/**
* \brief
* Creates an ETPP user defined trap, sets an action different than drop.
* Sets an MTU configuration and attaches it to the created trap, for RIF.
* \par DIRECT INPUT:
*   \param [in] unit         - Unit
*   \param [in] rif_intf     - RIF Interface.
*   \param [in] fwd_layer_type - Forwarding Layer Type
*   \param [in] compressed_layer_type - Compressed Forwarding Layer Type
*   \param [in] trap_id      - Trap ID.
*   \param [in] fwd_strength - Trap Strength
*   \param [in] snp_strength - Snoop Strength
*   \param [in] mtu_val      - MTU Value
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
int cint_rx_trap_etpp_mtu_trap_rif_main(
    int unit,
    bcm_if_t rif_intf,
    int fwd_layer_type,
    int compressed_layer_type,
    int trap_id,
    int fwd_strength,
    int snp_strength,
    int mtu_val,
    int mtu_profile)
{
    bcm_gport_t trap_gport;
    int rv = BCM_E_NONE;

    if(trap_id != BCM_RX_TRAP_EG_TX_TRAP_ID_DROP)
    {
        trap_config.is_recycle_append_ftmh = TRUE;

        rv = cint_utils_rx_trap_create_and_set(unit,BCM_RX_TRAP_WITH_ID, bcmRxTrapEgTxUserDefine, trap_config, &trap_id);
        if(rv != BCM_E_NONE)
        {
            printf("Error in cint_utils_rx_trap_create_and_set \n");
            return rv;
        }
    }

    cint_etpp_mtu_trap_rif_trap_id = trap_id;
    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    rv = cint_etpp_mtu_trap_mtu_rif_set(unit, rif_intf, trap_gport, mtu_val, fwd_layer_type, compressed_layer_type, mtu_profile);
    if(rv != BCM_E_NONE)
    {
        printf("Error in cint_etpp_mtu_trap_mtu_rif_set \n");
        return rv;
    }

    return rv;

}

/**
* \brief
* Set an MTU Configuration.
* Set the compressed fwd layer type.
* \par DIRECT INPUT:
*   \param [in] unit        - Unit
*   \param [in] rif_intf    - RIF ID
*   \param [in] trap_gport  - Trap Gport
*   \param [in] mtu_val     - MTU Value
*   \param [in] fwd_layer_type - Forward Layer type
*   \param [in] compressed_layer_type - Compressed forward layer type
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
int cint_etpp_mtu_trap_mtu_rif_set(
    int unit,
    bcm_if_t rif_intf,
    bcm_gport_t trap_gport,
    int mtu_val,
    int fwd_layer_type,
    int compressed_layer_type,
    int mtu_profile)
{
    bcm_rx_mtu_config_t mtu_config;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;
    int rv = BCM_E_NONE;


    bcm_rx_mtu_config_t_init(&mtu_config);
    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    /* Creating a compressed forwarding layer type */
    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = fwd_layer_type;

    info.value = compressed_layer_type;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_indexed_set \n");
        return rv;
    }

    mtu_key.cmp_layer_type = compressed_layer_type;
    mtu_key.mtu_profile = mtu_profile;

    mtu_value.mtu_val = mtu_val;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_RIF, &mtu_key, &mtu_value);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_mtu_profile_set \n");
        return rv;
    }

    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = rif_intf;
    mtu_config.mtu_profile = mtu_profile;

    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_mtu_set \n");
        return rv;
    }



    return rv;

}


/**
* \brief
* Destroys the created trap and MTU configuration for the supplied out-RIF.
* \par DIRECT INPUT:
*   \param [in] unit                        - Unit
*   \param [in] rif_intf                    - OUT-RIF value.
*   \param [in] fwd_layer_type              - Forward Layer type
*   \param [in] compressed_layer_type       - Compressed fwd layer type.
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
int cint_rx_trap_etpp_mtu_trap_rif_destroy(
    int unit,
    bcm_if_t rif_intf,
    int fwd_layer_type,
    int compressed_layer_type,
    int mtu_profile)
{
    bcm_rx_mtu_config_t  mtu_config;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;    
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;
    int rv = BCM_E_NONE;

    bcm_rx_mtu_config_t_init(&mtu_config);
    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    if(cint_etpp_mtu_trap_rif_trap_id != BCM_RX_TRAP_EG_TX_TRAP_ID_DROP)
    {
        /* delete trap */
        rv = bcm_rx_trap_type_destroy(unit, cint_etpp_mtu_trap_rif_trap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
            return rv;
        }
    }

    /* Setting compressed layer type to zero */
    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = fwd_layer_type;

    info.value = 0;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_indexed_set \n");
        return rv;
    }

    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = rif_intf;

    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_mtu_set, returned %d\n", rv);
        return rv;
    }

    mtu_key.cmp_layer_type = compressed_layer_type;
    mtu_key.mtu_profile = mtu_profile;

    mtu_value.mtu_val = 0;
    mtu_value.trap_gport = BCM_GPORT_INVALID;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_RIF, &mtu_key, &mtu_value);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_mtu_profile_set \n");
        return rv;
    }

    return rv;

}

