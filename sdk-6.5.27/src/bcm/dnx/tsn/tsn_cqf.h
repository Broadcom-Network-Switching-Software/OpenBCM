
#ifndef _TSN_CQF_H_INCLUDED_
#define _TSN_CQF_H_INCLUDED_

shr_error_e dnx_tsn_cqf_config_set(
    int unit,
    uint32 flags,
    bcm_port_t port,
    bcm_tsn_cqf_config_t * config);

shr_error_e dnx_tsn_cqf_config_get(
    int unit,
    uint32 flags,
    bcm_port_t port,
    bcm_tsn_cqf_config_t * config);

shr_error_e dnx_tsn_cqf_init(
    int unit);

shr_error_e dnx_tsn_cqf_port_deactivate_all(
    int unit);

shr_error_e dnx_tsn_cqf_port_base_time_restart(
    int unit);

#endif
