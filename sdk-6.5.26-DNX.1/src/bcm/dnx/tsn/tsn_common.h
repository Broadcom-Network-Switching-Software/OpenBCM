/**
 * \file src/bcm/dnx/tsn/tsn_common.h
 * 
 *
 * Time Sensitive Network common functions
 */
#ifndef _TSN_COMMON_H_INCLUDED_
#define _TSN_COMMON_H_INCLUDED_
/**
 * \brief - get egq port scheduler mode
 *
 * \param [in] unit - HW identifier of unit.
 * \param [in] port - logical port.
 * \param [out] ps_mode - egq port scheduler mode.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_common_port_to_ps_mode_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_egq_ps_mode_e * ps_mode);

/**
 * \brief - returns egress tsn interface
 *
 * \param [in] unit - HW identifier of unit.
 * \param [in] port - logical port.
 * \param [out] egr_tsn_if - egress tsn if.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_tsn_common_egr_tsn_interface_get(
    int unit,
    bcm_port_t port,
    int *egr_tsn_if);
#endif /* _TSN_COMMON_H_INCLUDED_ */
