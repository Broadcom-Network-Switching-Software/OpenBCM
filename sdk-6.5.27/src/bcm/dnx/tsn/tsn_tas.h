
#ifndef _TSN_TAS_H_INCLUDED_
#define _TSN_TAS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/port.h>
#include <bcm/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

typedef struct
{
    SHR_BITDCL bitmap[_SHR_BITDCLSIZE(DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT)];
} dnx_tsn_tas_pid_bmp_t;

#define DNX_TSN_TAS_PID_BMP_INIT(pid_bmp) \
    SHR_BITCLR_RANGE((pid_bmp)->bitmap, 0, DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT)

#define DNX_TSN_TAS_PID_BMP_IS_EMPTY(pid_bmp) \
    SHR_BITNULL_RANGE((pid_bmp)->bitmap, 0, DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT)

#define DNX_TSN_TAS_PID_BMP_BITSET(pid_bmp, pid)  \
    SHR_BITSET((pid_bmp)->bitmap, pid)

#define DNX_TSN_TAS_PID_BMP_IS_BITSET(pid_bmp, pid)  \
    SHR_IS_BITSET((pid_bmp)->bitmap, pid)

#define DNX_TSN_TAS_PID_BMP_BIT_ITER(pid_bmp, iter) \
    SHR_BIT_ITER((pid_bmp)->bitmap, DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT, iter)

shr_error_e dnx_tsn_tas_init(
    int unit);

shr_error_e dnx_tsn_tas_profile_state_active_set(
    int unit,
    bcm_core_t core_id,
    bcm_cosq_tas_profile_id_t pid);

shr_error_e dnx_tsn_tas_profile_multi_hw_update(
    int unit,
    bcm_core_t core_id,
    dnx_tsn_tas_pid_bmp_t * requested_bmp,
    dnx_tsn_tas_pid_bmp_t * updated_bmp);

shr_error_e dnx_tsn_tas_port_reset(
    int unit,
    bcm_core_t core_id,
    int tas_port);

shr_error_e dnx_tsn_tas_port_deactivate_all(
    int unit);

shr_error_e dnx_tsn_tas_port_base_time_restart(
    int unit);

#endif
