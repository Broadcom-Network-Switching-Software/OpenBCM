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

#endif  /*_PORT_PP_FLOW_INCLUDED__ */
