
#ifndef _SOC_RAMON_FABRIC_SYSTEM_UPGRADE_H_
#define _SOC_RAMON_FABRIC_SYSTEM_UPGRADE_H_

#ifndef BCM_DNXF1_SUPPORT
#error "This file is for use by DNXF1 (Ramon) family only!"
#endif

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/error.h>
#include <soc/types.h>

shr_error_e soc_ramon_system_upgrade_in_progress_get(
    int unit,
    int *in_progress);

shr_error_e soc_ramon_system_upgrade_enable_set(
    int unit,
    int enable);

shr_error_e soc_ramon_system_upgrade_phase1_init(
    int unit,
    int master_id);

shr_error_e soc_ramon_system_upgrade_target_set(
    int unit,
    uint32 target);

shr_error_e soc_ramon_system_upgrade_master_id_get(
    int unit,
    int *master_id);

shr_error_e soc_ramon_system_upgrade_enable_get(
    int unit,
    int *enable);

shr_error_e soc_ramon_system_upgrade_trig(
    int unit);

shr_error_e soc_ramon_system_upgrade_done_get(
    int unit,
    int *done);

shr_error_e soc_ramon_system_upgrade_ok_get(
    int unit,
    int *ok);

shr_error_e soc_ramon_system_upgrade_apply(
    int unit);

shr_error_e soc_ramon_system_upgrade_trig_get(
    int unit,
    int *is_triggered);

#endif
