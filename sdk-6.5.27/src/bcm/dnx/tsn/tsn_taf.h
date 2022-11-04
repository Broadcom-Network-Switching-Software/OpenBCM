
#ifndef _TSN_TAF_H_INCLUDED_
#define _TSN_TAF_H_INCLUDED_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

typedef struct
{
    SHR_BITDCL bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE)];
} dnx_tsn_taf_pid_bmp_t;

#define DNX_TSN_TAF_PID_BMP_INIT(pid_bmp) \
    SHR_BITCLR_RANGE((pid_bmp)->bitmap, 0, DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE)

#define DNX_TSN_TAF_PID_BMP_IS_EMPTY(pid_bmp) \
    SHR_BITNULL_RANGE((pid_bmp)->bitmap, 0, DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE)

#define DNX_TSN_TAF_PID_BMP_BITSET(pid_bmp, pid)  \
    SHR_BITSET((pid_bmp)->bitmap, pid)

#define DNX_TSN_TAF_PID_BMP_IS_BITSET(pid_bmp, pid)  \
    SHR_IS_BITSET((pid_bmp)->bitmap, pid)

#define DNX_TSN_TAF_PID_BMP_BIT_ITER(pid_bmp, iter) \
    SHR_BIT_ITER((pid_bmp)->bitmap, DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE, iter)

shr_error_e dnx_tsn_taf_init(
    int unit);

shr_error_e dnx_tsn_taf_gate_create(
    int unit,
    int flags,
    int *taf_gate_id);

shr_error_e dnx_tsn_taf_gate_destroy(
    int unit,
    int taf_gate_id);

shr_error_e dnx_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t * profile,
    bcm_tsn_taf_profile_id_t * pid);

shr_error_e dnx_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid);

shr_error_e dnx_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile);

shr_error_e dnx_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile);

shr_error_e dnx_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid);

shr_error_e dnx_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status);

shr_error_e dnx_tsn_taf_control_reset(
    int unit,
    int taf_gate,
    int arg);

shr_error_e dnx_tsn_taf_profile_state_active_set(
    int unit,
    bcm_tsn_taf_profile_id_t pid);

shr_error_e dnx_tsn_taf_profile_multi_hw_update(
    int unit,
    dnx_tsn_taf_pid_bmp_t * requested_bmp,
    dnx_tsn_taf_pid_bmp_t * updated_bmp);

shr_error_e dnx_tsn_taf_gate_reset(
    int unit,
    int taf_gate);

shr_error_e dnx_tsn_taf_gate_deactivate_all(
    int unit);

shr_error_e dnx_tsn_taf_gate_base_time_restart(
    int unit);

#endif
