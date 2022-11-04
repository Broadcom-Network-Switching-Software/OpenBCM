/*
 * File: cint_rx_trap_etpp_mtu_trap_port.c
 * Purpose: Configuration that creates an ETPP User defined trap.
 *          Creates an MTU violation configuration PER PORT and attaches it,
 *          to the created ETPP trap.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_mtu_trap_port.c
 *
 * Main Function:
 *      cint_rx_trap_etpp_mtu_trap_port_main(unit,port,is_pkt_drop,fwd_strength,snp_strength,mtu_val);
 * Destroy Function:
 *      cint_rx_trap_etpp_mtu_trap_port_destroy(unit,port);
 *
 * Example Config requires a valid(created) out_lif to be supplied:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
  cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_mtu_trap_lif.c
  cint
  cint_rx_trap_etpp_mtu_trap_port_main(0,200,TRUE,7,0,0x20);
 * Test Scenario - end
 */


/* The encoded User defined trap id */
int cint_etpp_mtu_trap_port_trap_id;
bcm_rx_trap_config_t cint_etpp_mtu_trap_port_trap_config = {0};

BCM_GPORT_TRAP_SET(cint_etpp_mtu_trap_port_trap_config.cpu_trap_gport,0,0,0);
cint_etpp_mtu_trap_port_trap_config.is_recycle_append_ftmh = 1;

/**
* \brief
* Creates an ETPP user defined trap, sets an action different than drop.
* Sets an MTU configuration and attaches it to the created trap, for PORT.
* \par DIRECT INPUT:
*   \param [in] unit         - Unit
*   \param [in] port         - Port value.
*   \param [in] is_pkt_drop  - Indication if to set packet action to drop.
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
int cint_rx_trap_etpp_mtu_trap_port_main(
    int unit,
    bcm_gport_t port,
    int is_pkt_drop,
    int fwd_strength,
    int snp_strength,
    int mtu_val,
    int mtu_profile)
{
    bcm_gport_t trap_gport;

    if(is_pkt_drop != TRUE)
    {
        BCM_IF_ERROR_RETURN_MSG(cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrapEgTxUserDefine, &cint_etpp_mtu_trap_port_trap_config, &cint_etpp_mtu_trap_port_trap_id), "");
    }
    else
    {
        cint_etpp_mtu_trap_port_trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DROP;
    }

    BCM_GPORT_TRAP_SET(trap_gport, cint_etpp_mtu_trap_port_trap_id, fwd_strength, snp_strength);
    /* The FWD layer type does not matter for per port MTU config. */
    BCM_IF_ERROR_RETURN_MSG(cint_etpp_mtu_trap_mtu_port_set(unit, port, trap_gport, mtu_val, mtu_profile), "");

    return BCM_E_NONE;
}

/**
* \brief
* Set an MTU Configuration.
* \par DIRECT INPUT:
*   \param [in] unit        - Unit
*   \param [in] port        - PP Port
*   \param [in] trap_gport  - Trap Gport
*   \param [in] mtu_val     - MTU Value
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
int cint_etpp_mtu_trap_mtu_port_set(
    int unit,
    bcm_gport_t port,
    bcm_gport_t trap_gport,
    int mtu_val,
    int mtu_profile)
{
    bcm_rx_mtu_config_t mtu_config;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;

    bcm_rx_mtu_config_t_init(&mtu_config);
    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = mtu_profile;

    mtu_value.mtu_val = mtu_val;
    mtu_value.trap_gport = trap_gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_PORT, &mtu_key, &mtu_value), "");

    /* Setting the MTU config values */
    mtu_config.flags = BCM_RX_MTU_PORT;
    mtu_config.gport = port;
    mtu_config.mtu_profile = mtu_profile;

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_mtu_set(unit, &mtu_config), "");

    return BCM_E_NONE;
}

/**
* \brief
* Destroys the created trap and MTU configuration for the supplied port.
* \par DIRECT INPUT:
*   \param [in] unit         - Unit
*   \param [in] port         - Port value.
*   \param [in] mtu_profile  - MTU profile.
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
int cint_rx_trap_etpp_mtu_trap_port_destroy(
    int unit,
    bcm_gport_t port,
    int mtu_profile)
{
    bcm_rx_mtu_config_t  mtu_config;
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;

    bcm_rx_mtu_config_t_init(&mtu_config);
    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    if(cint_etpp_mtu_trap_port_trap_id != BCM_RX_TRAP_EG_TX_TRAP_ID_DROP)
    {
        /* delete trap */
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_destroy(unit, cint_etpp_mtu_trap_port_trap_id), "");
    }
    mtu_config.flags = BCM_RX_MTU_PORT;
    mtu_config.gport = port;

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_mtu_set(unit, &mtu_config), "");

    mtu_key.mtu_profile = mtu_profile;

    BCM_IF_ERROR_RETURN_MSG(bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_RIF, &mtu_key, &mtu_value), "");

    return BCM_E_NONE;
}

