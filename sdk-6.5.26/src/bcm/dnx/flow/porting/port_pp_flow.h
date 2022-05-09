/** \file port_pp_flow.h
 * Port pp using flow lif
*/

#ifndef _PORT_PP_FLOW_INCLUDED__
/*
 * {
 */
#define _PORT_PP_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

/**
 * \brief -
 * Set the relevant field of the LIF entry with the given value per the port class.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] pclass - port class, used to indicate the right field
 * \param [in] value - value to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_class_flow_lif_property_set(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_class_t pclass,
    uint32 value);

/**
 * \brief -
 * Get the relevant field value from the LIF entry per the port class.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] pclass - port class, used to indicate the right field
 * \param [out] value - Pointer for value to get
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_class_flow_lif_property_get(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_class_t pclass,
    uint32 *value);

/**
 * \brief -
 * Set the relevant field of the LIF entry with the given value per the port control.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] port_control_type - port control, used to indicate the right field
 * \param [in] value - value to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_control_flow_lif_property_set(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_control_t port_control_type,
    int value);

/**
 * \brief -
 * Get the relevant field value from the LIF entry per the port control.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] port_control_type - port control, used to indicate the right field
 * \param [out] value - Pointer for value to get
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_control_flow_lif_property_get(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_control_t port_control_type,
    int *value);

/**
 * \brief -
 *  Set the flooding groups for unknown packets to an InLIF.
 *
 * \param [in] unit - The unit ID.
 * \param [in] gport_lif - Gport for lif-id.
 * \param [in] flood_groups - The flooding destinations for unknown packets.
 *
 * \return
 *  \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *   dnx_port_lif_flood_profile_set.
 */
shr_error_e dnx_port_pp_flow_lif_flood_set(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_flood_group_t * flood_groups);

/**
 * \brief -
 *  Get the flooding groups for unknown packets from an InLIF.
 *
 * \param [in] unit - The unit ID.
 * \param [in] gport_lif - Gport for lif-id.
 * \param [out] flood_groups - The flooding destinations for unknown packets.
 *
 * \return
 *  \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *   bcm_dnx_port_flood_group_get.
 */
shr_error_e dnx_port_pp_flow_lif_flood_get(
    int unit,
    bcm_gport_t gport_lif,
    bcm_port_flood_group_t * flood_groups);

/**
 * \brief -
 *  Set the learning mode based on the flags parameter to a logical port.
 *  Use the flow lif framework to write the learn_enable data to the related flow application.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port refer to a lif in gport format.
 * \param [in] flags - Flags for learning mode. Possible values:
 *                     BCM_PORT_LEARN_ARL
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_flow_lif_learn_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags);

/**
 * \brief -
 *  Get the learning mode from a logical port.
 *  Use the flow lif framework to read the learn_enable data from the related flow application.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port refer to a lif in gport format.
 * \param [out] flags - Flags for learning mode. Possible values:
 *                      BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_flow_lif_learn_get(
    int unit,
    bcm_gport_t gport,
    uint32 *flags);

#endif  /*_PORT_PP_FLOW_INCLUDED__ */
