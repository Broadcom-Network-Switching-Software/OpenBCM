/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.c
 * Purpose:     Implementation of bcm multiplexing
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/shared/mbcm_sat.h>
#ifdef BCM_SAT_SUPPORT

CONST mbcm_sat_functions_t    *mbcm_sat_driver[BCM_MAX_NUM_UNITS]={0};
soc_sat_chip_family_t    mbcm_sat_family[BCM_MAX_NUM_UNITS];

CONST mbcm_sat_functions_t mbcm_sat_fun_driver = {
    soc_sat_config_set,
    soc_sat_config_get,
    /* GTF functions */
    soc_sat_general_cfg_init,
    soc_sat_gtf_packet_gen_set,
    soc_sat_gtf_packet_gen_get,
    soc_sat_gtf_packet_config,
    soc_sat_gtf_bandwidth_set,
    soc_sat_gtf_bandwidth_get,
    soc_sat_gtf_rate_pattern_set,
    soc_sat_gtf_rate_pattern_get,
    soc_sat_gtf_stat_get,
    /* CTF functions */
    soc_sat_ctf_packet_config,
    soc_sat_ctf_identifier_map,
    soc_sat_ctf_identifier_unmap,
    soc_sat_ctf_trap_set,
    soc_sat_ctf_bin_limit_set,
    soc_sat_ctf_bin_limit_get,
    soc_sat_ctf_stat_config_set,
    soc_sat_ctf_stat_get,
    soc_sat_ctf_availability_config_set,
    soc_sat_ctf_trap_data_to_session_map,
    soc_sat_ctf_trap_data_to_session_unmap,
    soc_sat_ctf_reports_config_set,
    /* OAMP interrupt */
    soc_sat_oamp_stat_event_control,
    soc_sat_oamp_stat_event_clear,
    soc_sat_session_inuse,
};


/****************************************************************
 *
 * Function:        mbcm_sat_init
 * Parameters:      unit   --   unit to setup
 * 
 * Initialize the mbcm  sat driver for the indicated unit.
 */
int
mbcm_sat_init(int unit)
{
#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_JERICHO(unit)) {
        mbcm_sat_driver[unit] = &mbcm_sat_fun_driver;
        mbcm_sat_family[unit] = SOC_SAT_FAMILY_JERICHO;
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            mbcm_sat_driver[unit] = &mbcm_sat_fun_driver;
            mbcm_sat_family[unit] = SOC_SAT_FAMILY_SABER2;
            return SOC_E_NONE;
        }
#endif /* BCM_SABER2_SUPPORT */


    LOG_INFO(BSL_LS_BCM_INIT,
             (BSL_META_U(unit,
                         "ERROR: mbcm_sat_init unit %d: unsupported chip type\n"), unit));
    return SOC_E_UNAVAIL;
}
#endif /* BCM_SAT_SUPPORT  */
