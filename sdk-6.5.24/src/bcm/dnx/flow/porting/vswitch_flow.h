/** \file vswitch_flow.h
 * vswitch using flow lif
*/

#ifndef _VSWITCH_FLOW_INCLUDED__
/*
 * {
 */
#define _VSWITCH_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

/**
 * \brief -
 * Attachs given 2 ports in P2P service using LIF Flow
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the two ports to attached to P2P
 *        service:
 *          - port1 - First gport in cross connect.
 *          - port2 - Second gport in cross connect.
 *          - encap1 - First gport encap id.
 *          - encap2 - Second gport encap id.
 *          - flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_vswitch_cross_connect_add_flow(
    int unit,
    bcm_vswitch_cross_connect_t * gports);

/**
 * \brief -
 * For a given P2P gport, returns its P2P peer using LIF Flow
 * \param [in] unit - relevant unit.
 * \param [in, out] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the port given and will contain information
 *        on peer port on return.
 *        service:
 *          - [in] port1 - given port.
 *          - [out] port2 - peer of port1, retun value.
 *          - [in] encap1 - First gport encap id.
 *          - [out] encap2 - Second gport encap id.
 *          - [in] flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vswitch_cross_connect_get_flow(
    int unit,
    bcm_vswitch_cross_connect_t * gports);
/**
 * \brief -
 *  delete cross connect entry for a given P2P gport using LIF Flow
 * \param [in] unit - relevant unit.
 * \param [in] gports - pointer to structure type
 *        bcm_vswitch_cross_connect_t which holds the
 *        information about the port given.
 *        service:
 *          - [in] port1 - given port.
 *          - [in] port2 - if flags=0, 2nd given port to delete.
 *          - [in] flags - BCM_VSWITCH_CROSS_CONNECT_XXX flags.
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_vswitch_cross_connect_delete_flow(
    int unit,
    bcm_vswitch_cross_connect_t * gports);

/**
 * \brief -
 * BCM API: Traverse on all existing P2P services and preform given Callback function.
 * \param [in] unit - relevant unit.
 * \param [in] cb - Callback function to perform
 * \param [in] user_data - pointer to additional user data.
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vswitch_cross_connect_traverse_flow(
    int unit,
    bcm_vswitch_cross_connect_traverse_cb cb,
    void *user_data);

/**
 * \brief -
 * BCM API: Traverse on all existing P2P services and delete.
 * \param [in] unit - relevant unit.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vswitch_cross_connect_traverse_delete_flow(
    int unit);
#endif  /*_VSWITCH_FLOW_INCLUDED__ */
