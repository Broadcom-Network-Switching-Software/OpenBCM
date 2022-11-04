
#ifndef _TSN_THREAD_H_INCLUDED_
#define _TSN_THREAD_H_INCLUDED_

shr_error_e dnx_tsn_thread_db_init(
    int unit);

shr_error_e dnx_tsn_thread_profile_commit(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    dnx_tsn_gate_type_e type,
    int pid);

shr_error_e dnx_tsn_thread_control_reset(
    int unit,
    dnx_tsn_gate_type_e type,
    int gate_id);

shr_error_e dnx_tsn_thread_tas_tod_update_interrupt_handler(
    int unit);

#endif
