
#ifndef _TSN_COMMON_H_INCLUDED_
#define _TSN_COMMON_H_INCLUDED_

shr_error_e dnx_tsn_common_port_to_ps_mode_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_egq_ps_mode_e * ps_mode);

shr_error_e dnx_tsn_common_egr_tsn_interface_get(
    int unit,
    bcm_port_t port,
    int *egr_tsn_if);
#endif
